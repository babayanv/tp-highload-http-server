#include "http_server/http/request.hpp"


namespace http {


std::tuple<Request, status::value_type> build_request(const std::string_view& request_sv, const std::string_view& doc_root_sv) {
    RequestBuilder rb;
    return rb(request_sv, doc_root_sv);
}


std::tuple<Request, status::value_type> RequestBuilder::operator()(const std::string_view& request_sv, const std::string_view& doc_root_sv) {
    m_request_sv = request_sv;

    get_method();
    get_path();
    get_version();

    validate_method() &&
    validate_path(doc_root_sv) &&
    validate_version();

    return std::make_tuple(m_request, m_status);
}


void RequestBuilder::get_method() {
    size_t pos = m_request_sv.find(" /");

    m_request.method = m_request_sv.substr(0, pos);
    m_request_sv.remove_prefix(pos + 1);
}


void RequestBuilder::get_path() {
    size_t pos = m_request_sv.find(" HTTP/");

    m_request.path = m_request_sv.substr(0, pos);
    m_request_sv.remove_prefix(pos + 1);
}


void RequestBuilder::get_version() {
    size_t pos = m_request_sv.find(CRLF);

    m_request.version = m_request_sv.substr(0, pos);
    m_request_sv.remove_prefix(pos + CRLF.size());
}


bool RequestBuilder::validate_method() {
    if (m_request.method != method::M_GET &&
        m_request.method != method::M_HEAD)
    {
        m_status = status::S_405_MNA;
        return false;
    }

    return true;
}


bool RequestBuilder::validate_path(const std::string_view& doc_root_sv) {
    if (m_request.path.find("..") != std::string::npos) {
        m_status = status::S_403_F;
        return false;
    }

    m_request.path = doc_root_sv.data() + m_request.path;
    if (m_request.path.ends_with('/')) {
        m_request.path += "index.html";
    }

    //decode url encoding
    for (size_t url_encode_pos = m_request.path.find('%'); url_encode_pos != std::string::npos; url_encode_pos = m_request.path.find('%')) {
        std::string_view url_encode = static_cast<std::string_view>(m_request.path).substr(url_encode_pos + 1, 2);

        char url_decode = static_cast<char>(std::stoul(url_encode.data(), nullptr, 16));
        if (url_decode == 0) {
            continue;
        }

        m_request.path.replace(url_encode_pos, 3, 1, url_decode);
    }
}


bool RequestBuilder::validate_version() {
    if (!m_request.version.starts_with("HTTP/")) {
        m_status = status::S_400_BR;
        return false;
    }

    return true;
}


} // namespace http