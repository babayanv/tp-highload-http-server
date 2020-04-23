#include "http_server/exception.hpp"

#include <cstring>


ErrnoException::ErrnoException()
        : m_msg(std::strerror(errno))
{
}


ErrnoException::ErrnoException(const std::string& what_arg)
        : m_msg(what_arg)
{
    m_msg += std::strerror(errno);
}


const char* ErrnoException::what() const noexcept {
    return m_msg.c_str();
}
