#ifndef HTTP_RESPONSE_HPP
#define HTTP_RESPONSE_HPP

#include "http_server/http/http.hpp"
#include "http_server/http/request.hpp"
#include "http_server/connection.hpp"

#include <sstream>


namespace http {


void build_and_send_response(Connection& conn, const Request& request, status::value_type& status);


class ResponseBuilder {
public:
    ResponseBuilder() = default;

    void operator()(Connection& conn, const Request& request, status::value_type& status);

private:
    std::ostringstream m_oss;

private:
    void check_file_availability(const std::string_view& file_path, status::value_type& status);
    void write_headers(const Request& request, status::value_type& status);
};


} // namespace http

#endif // HTTP_RESPONSE_HPP
