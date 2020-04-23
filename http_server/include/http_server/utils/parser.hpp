#ifndef UTILS_PARSER_HPP
#define UTILS_PARSER_HPP

#include <tuple>


namespace utils {

std::tuple<std::string_view, std::string_view, uint16_t, size_t> parse_args(char* argv[]);
std::tuple<size_t, size_t, std::string> parse_config(const std::string_view& config_path);

} // namespace utils

#endif // UTILS_PARSER_HPP