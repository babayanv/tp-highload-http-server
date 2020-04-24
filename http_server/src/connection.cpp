#include "http_server/connection.hpp"
#include "http_server/exception.hpp"

#include <sys/socket.h>
#include <unistd.h>

#include <sstream>


Connection::Connection(int sock_fd)
        : m_sock_fd(sock_fd)
{
}


Connection::Connection(Connection&& other) noexcept
        : m_sock_fd(other.m_sock_fd.extract())
{
}


Connection& Connection::operator=(Connection&& other) noexcept {
    close();

    m_sock_fd = other.m_sock_fd.extract();

    return *this;
}


void Connection::close()
{
    m_sock_fd.close();
}


size_t Connection::write(const void* data, size_t len) const
{
    while (true)
    {
        ssize_t bytes_written = ::write(m_sock_fd, data, len);
        if (bytes_written < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }
            if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                return 0;
            }

            throw ConnectionError("Error writing to socket: ");
        }

        return static_cast<size_t>(bytes_written);
    }
}


void Connection::write_exact(const void* data, size_t len) const
{
    size_t bytes_written_total = 0;

    while (bytes_written_total != len)
    {
        const void* buff_begin = static_cast<const char*>(data) + bytes_written_total;

        size_t bytes_written = write(buff_begin, len - bytes_written_total);
        if (bytes_written == 0)
        {
            throw ConnectionError("Unable to write exactly " + std::to_string(len) + " bytes: ");
        }

        bytes_written_total += bytes_written;
    }
}



size_t Connection::read(void* data, size_t len)
{
    while (true)
    {
        ssize_t bytes_read = ::read(m_sock_fd, data, len);

        if (bytes_read < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }
            if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                return 0;
            }

            throw ConnectionError("Error reading from socket: ");
        }

        return static_cast<size_t>(bytes_read);
    }
}


void Connection::read_exact(void* data, size_t len)
{
    size_t bytes_read_total = 0;

    while (bytes_read_total < len)
    {
        void* buff_begin = static_cast<char*>(data) + bytes_read_total;

        size_t bytes_read = read(buff_begin, len - bytes_read_total);
        if (bytes_read == 0)
        {
            throw ConnectionError("Unable to read exactly " + std::to_string(len) + " bytes: ");
        }

        bytes_read_total += bytes_read;
    }
}


std::string Connection::read_all(long limit) {
    std::ostringstream oss;

    while (oss.tellp() < limit) {
        std::string chunk(1024, '\0');

        size_t bytes_read = read(chunk.data(), chunk.size());

        oss.write(chunk.c_str(), static_cast<long>(bytes_read));

        if (bytes_read < chunk.size()) {
            break;
        }
    }

    return oss.str();
}
