#include "http_server/exception.hpp"

#include <cstring>
#include <utility>


ErrnoException::ErrnoException()
    : m_msg(std::strerror(errno))
{
}


ErrnoException::ErrnoException(std::string  what_arg)
    : m_msg(std::move(what_arg))
{
    m_msg += std::strerror(errno);
}


const char* ErrnoException::what() const noexcept {
    return m_msg.c_str();
}
