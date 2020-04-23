#include "http_server/utils/parser.hpp"

#include <charconv>
#include <fstream>


namespace utils {


constexpr std::string_view CPU_LIMIT_OPTION = "cpu_limit";
constexpr std::string_view THREAD_LIMIT_OPTION = "thread_limit";
constexpr std::string_view DOC_ROOT_OPTION = "doc_root";


std::tuple<std::string_view, std::string_view, uint16_t, size_t> parse_args(char* argv[]) {
    uint16_t port{};
    {
        std::string_view sv(argv[3]);
        std::from_chars(sv.data(), sv.data() + sv.size(), port);
    }

    size_t max_connect{};
    {
        std::string_view sv(argv[4]);
        std::from_chars(sv.data(), sv.data() + sv.size(), max_connect);
    }

    return std::make_tuple(argv[1], argv[2], port, max_connect);
}


std::tuple<size_t, size_t, std::string> parse_config(const std::string_view& config_path) {
    std::ifstream ifs(config_path.data());

    size_t cpu_limit = 0;
    size_t thread_limit = 0;
    std::string doc_root{};

    while (ifs) {
        std::string option;
        ifs >> option;

        if (option == CPU_LIMIT_OPTION) {
            ifs >> cpu_limit;
        }
        else if (option == THREAD_LIMIT_OPTION) {
            ifs >> thread_limit;
        }
        else if (option == DOC_ROOT_OPTION) {
            ifs >> doc_root;
        }
    }

    return std::make_tuple(cpu_limit, thread_limit, doc_root);
}


} // namespace utils
