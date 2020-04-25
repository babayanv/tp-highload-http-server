#ifndef EXCEPTION_HPP
#define EXCEPTION_HPP

#include <exception>
#include <string>


class ErrnoException : public std::exception
{
public:
    ErrnoException();
    explicit ErrnoException(std::string what_arg);

    ~ErrnoException() noexcept override = default;

    [[nodiscard]] const char* what() const noexcept override;

private:
    std::string m_msg;
};


class FDError : public ErrnoException
{
public:
    FDError() = default;
    explicit FDError(const std::string& what_arg) : ErrnoException(what_arg) {}
};


class ConnectionError : public ErrnoException
{
public:
    ConnectionError() = default;
    explicit ConnectionError(const std::string& what_arg) : ErrnoException(what_arg) {}
};


class ServerError : public ErrnoException
{
public:
    ServerError() = default;
    explicit ServerError(const std::string& what_arg) : ErrnoException(what_arg) {}
};

#endif // EXCEPTION_HPP
