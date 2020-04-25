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

    Connection& operator=(Connection&& other) noexcept;

    Connection(const Connection& other) = delete;
    Connection& operator=(const Connection& other) = delete;

    void close();

    size_t write(const void* data, size_t len) const;
    void write_exact(const void* data, size_t len) const;
    size_t read(void* data, size_t len);
    std::string read_all(long limit);

    size_t send_file(std::string_view filename);

private:
    utils::FileDescriptor m_sock_fd;
};

#endif // CONNECTION_HPP
