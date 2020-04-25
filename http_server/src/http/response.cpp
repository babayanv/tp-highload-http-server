#include "http_server/http/response.hpp"

#include <fstream>
#include <filesystem>


namespace http {


void build_and_send_response(Connection& conn, const Request& request, status::value_type& status) {
    ResponseBuilder rb;
    return rb(conn, request, status);
}


void ResponseBuilder::operator()(Connection& conn, const Request& request, status::value_type& status) {
    namespace fs = std::filesystem;

    check_file_availability(request.path, status);

    write_headers(request, status);

    conn.write_exact(m_oss.str().c_str(), static_cast<size_t>(m_oss.tellp()));

    if (status == status::S_200_OK && request.method == method::M_GET) {
        conn.send_file(request.path);
    }
}


void ResponseBuilder::check_file_availability(const std::string_view& file_path, status::value_type& status) {
    namespace fs = std::filesystem;

    if (status == status::S_200_OK && !fs::exists(file_path)) {
        if (file_path.ends_with("index.html")) {
            status = status::S_403_F;
        }
        else {
            status = status::S_404_NF;
        }
    }
}


void ResponseBuilder::write_headers(const Request& request, status::value_type& status) {
    namespace fs = std::filesystem;

    m_oss << request.version << ' ' << status << CRLF
          << "Server: tp-highload-http" << CRLF
          << "Date: " << __DATE__ << ' ' << __TIME__ << CRLF
          << "Connection: Close" << CRLF;

    if (status == status::S_200_OK) {
        m_oss << "Content-Length: " << fs::file_size(request.path) << CRLF
              << "Content-Type: " << determine_mime_type(request.path) << CRLF;
    }

    m_oss << CRLF;
}


} // namespace http
