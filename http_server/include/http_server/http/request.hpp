#ifndef HTTP_REQUEST_HPP
#define HTTP_REQUEST_HPP

#include "http_server/http/http.hpp"

#include <tuple>


namespace http {


struct Request {
    method::value_type method;
    std::string path;
    std::string_view version;
};


std::tuple<Request, status::value_type> build_request(const std::string_view& request_sv, const std::string_view& doc_root_sv);


class RequestBuilder {
public:
    RequestBuilder() = default;

    std::tuple<Request, status::value_type> operator()(const std::string_view& request_sv, const std::string_view& doc_root_sv);

private:
    std::string_view m_request_sv;
    Request m_request{};
    status::value_type m_status = status::S_200_OK;

private:
    void get_method();
    void get_path();
    void get_version();

    bool validate_method();
    bool validate_path(const std::string_view& doc_root_sv);
    bool validate_version();
};


} // namespace http

#endif // HTTP_REQUEST_HPP
