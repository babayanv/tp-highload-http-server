#ifndef HTTP_HTTP_HPP
#define HTTP_HTTP_HPP

#include <string_view>


namespace http {

namespace status {
    using value_type = std::string_view;
    constexpr value_type S_200_OK = "200 OK";
    constexpr value_type S_400_BR = "400 Bad Request";
    constexpr value_type S_403_F = "403 Forbidden";
    constexpr value_type S_404_NF = "404 Not Found";
} // namespace status

namespace mime_type {
    using value_type = std::string_view;
    constexpr value_type MT_HTML = "text/html";
    constexpr value_type MT_PLAIN = "text/plain";
    constexpr value_type MT_CSS = "text/css";
    constexpr value_type MT_GIF = "image/gif";
    constexpr value_type MT_PNG = "image/png";
    constexpr value_type MT_JPG = "image/jpeg";
    constexpr value_type MT_TIFF = "image/tiff";
    constexpr value_type MT_BMP = "image/bmp";
    constexpr value_type MT_JS = "application/javascript";
    constexpr value_type MT_SWF = "application/x-shockwave-flash";
} // namespace mime_type

    constexpr std::string_view CRLF = "\r\n";

    mime_type::value_type determine_mime_type(const std::string_view& file_path);

} // namespace http

#endif // HTTP_HTTP_HPP
