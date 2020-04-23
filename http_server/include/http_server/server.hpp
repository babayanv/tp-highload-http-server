#ifndef SERVER_HPP
#define SERVER_HPP

#include "http_server/utils/file_descriptor.hpp"
#include "http_server/connection.hpp"

#include <thread>
#include <vector>
#include <condition_variable>

#include <boost/lockfree/queue.hpp>


class Server {
    using ThreadPool = std::vector<std::thread>;

public:
    Server(const Server& other) = delete;
    Server(Server&& other) = delete;
    Server& operator=(const Server& other) = delete;
    Server& operator=(Server&& other) = delete;

    static Server& start(const std::string_view& address, uint16_t port, size_t max_connect, size_t thread_limit, const std::string& doc_root);
    static Server& get_instance();

    void open(const std::string_view& address, uint16_t port);
    void listen(int max_connect) const;

    void close();
    void join_workers();

    [[nodiscard]] bool is_opened() const noexcept;

    void run();

private:
    utils::FileDescriptor m_sock_fd;
    utils::FileDescriptor m_epoll_fd;

    ThreadPool m_workers;
    std::condition_variable m_cv;
    bool m_done = false;
    boost::lockfree::queue<int> m_spmc_queue;

private:
    Server(const std::string_view& address, uint16_t port, size_t max_connect, size_t thread_limit, const std::string& doc_root);
    ~Server() noexcept;

    static void handle_signal(int);

    void create_epoll();
    void add_epoll(int fd, uint32_t events) const;
    void accept_clients();
    void handle_client(int fd, epoll_event event);

    void work(const std::string& doc_root);
};

#endif // SERVER_HPP
