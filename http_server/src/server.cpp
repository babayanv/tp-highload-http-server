#include "http_server/server.hpp"
#include "http_server/exception.hpp"
#include "http_server/http/request.hpp"
#include "http_server/http/response.hpp"

#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <csignal>


Server& Server::start(const std::string_view& address, uint16_t port, size_t max_connect, size_t thread_limit, const std::string& doc_root) {
    static Server s_instance(address, port, max_connect, thread_limit, doc_root);
    return s_instance;
}


Server& Server::get_instance() {
    return start("", 0, 0, 0, "");
}


Server::Server(const std::string_view& address, uint16_t port, size_t max_connect, size_t thread_limit, const std::string& doc_root)
    : m_spmc_queue(max_connect)
{
    std::signal(SIGINT, handle_signal);
    std::signal(SIGTERM, handle_signal);
    std::signal(SIGABRT, handle_signal);

    for (size_t i = 0; i < thread_limit; ++i) {
        m_workers.emplace_back([this, doc_root] { this->work(doc_root); });
    }

    try
    {
        open(address, port);
        listen(static_cast<int>(max_connect));
        create_epoll();
        add_epoll(m_sock_fd, EPOLLIN);
    }
    catch (const ServerError& se)
    {
        close();
        throw se;
    }
}


Server::~Server() noexcept {
    join_workers();
}


void Server::open(const std::string_view& address, uint16_t port)
{
    if (is_opened())
    {
        close();
    }

    m_sock_fd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (m_sock_fd < 0)
    {
        throw ServerError("Error creating socket: ");
    }

    sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = ::htons(port);
    if (::inet_aton(address.data(), &serv_addr.sin_addr) == 0)
    {
        throw ServerError("Invalid ipv4 address: ");
    }

    if (::bind(m_sock_fd,
               reinterpret_cast<sockaddr*>(&serv_addr),
               sizeof(serv_addr)) != 0)
    {
        throw ServerError("Error binding socket: ");
    }
}


void Server::listen(int max_connect) const
{
    if (::listen(m_sock_fd, max_connect) != 0)
    {
        throw ServerError("Socket listen failed: ");
    }
}


void Server::close()
{
    m_sock_fd.close();
    m_epoll_fd.close();
}


void Server::join_workers() {
    if (m_done) {
        return;
    }

    m_done = true;
    m_cv.notify_all();

    for (auto& i : m_workers) {
        i.join();
    }
}


bool Server::is_opened() const noexcept
{
    return m_sock_fd.is_opened();
}


void Server::run()
{
    constexpr size_t EPOLL_SIZE = 128;
    epoll_event events[EPOLL_SIZE];

    while (is_opened())
    {
        int fd_count = epoll_wait(m_epoll_fd, events, EPOLL_SIZE, -1);
        if (fd_count < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }

            throw ServerError("Error waiting epoll: ");
        }

        for (int i = 0; i < fd_count; ++i)
        {
            int fd = events[i].data.fd;

            if (fd == m_sock_fd)
            {
                accept_clients();
            }
            else
            {
                handle_client(fd, events[i]);
            }
        }
    }
}


void Server::handle_signal(int) {
    get_instance().join_workers();
    get_instance().close();
}


void Server::create_epoll()
{
    m_epoll_fd = epoll_create(1);
    if (m_epoll_fd < 0)
    {
        throw ServerError("Error creating epoll: ");
    }
}


void Server::add_epoll(int fd, uint32_t events) const
{
    epoll_event event{};
    event.data.fd = fd;
    event.events = events;

    if (epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, fd, &event) < 0)
    {
        throw ServerError("Error adding fd to epoll: ");
    }
}


void Server::accept_clients()
{
    while (true)
    {
        sockaddr_in client_addr{};
        socklen_t addr_size = sizeof(client_addr);

        int conn_fd = ::accept4(m_sock_fd,
                                reinterpret_cast<sockaddr*>(&client_addr),
                                &addr_size,
                                SOCK_NONBLOCK);
        if (conn_fd < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }
            if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                return;
            }

            throw ServerError("Error accepting connection: ");
        }

        add_epoll(conn_fd, EPOLLIN | EPOLLRDHUP | EPOLLET);
    }
}


void Server::handle_client(int fd, epoll_event event)
{
    if (event.events & EPOLLHUP ||
        event.events & EPOLLERR ||
        event.events & EPOLLRDHUP)
    {
        return;
    }

    m_spmc_queue.push(fd);
    m_cv.notify_one();
}


void Server::work(const std::string& doc_root) {
    std::mutex mut;

    while (!m_done) {
        std::unique_lock lock(mut);
        m_cv.wait(lock,
            [this]{
                return !m_spmc_queue.empty() || m_done;
            }
        );

        if (m_done) {
            break;
        }

        int fd;
        if (!m_spmc_queue.pop(fd)) {
            continue;
        }

        Connection conn(fd);

        std::string msg = conn.read_all(4096);

        auto [ request, status ] = http::build_request(msg, doc_root);

        http::build_and_send_response(conn, request, status);
    }
}
