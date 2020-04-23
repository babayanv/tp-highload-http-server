#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include "http_server/utils/file_descriptor.hpp"

#include <netinet/ip.h>
#include <sys/epoll.h>

#include <string>
#include <list>
#include <vector>


class Connection {
public:
    explicit Connection(int sock_fd);
    Connection(Connection&& other) noexcept;

    ~Connection() noexcept = default;

    Connection& operator=(Connection&& other);

    Connection(const Connection& other) = delete;
    Connection& operator=(const Connection& other) = delete;

    void close();
    [[nodiscard]] bool is_opened() const noexcept;

    size_t write(const void* data, size_t len) const;
    void write_exact(const void* data, size_t len) const;
    size_t read(void* data, size_t len);
    void read_exact(void* data, size_t len);
    std::string read_all(size_t limit);

private:
    utils::FileDescriptor m_sock_fd;
};

#endif // CONNECTION_HPP
