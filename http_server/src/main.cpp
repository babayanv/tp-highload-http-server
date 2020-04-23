#include "http_server/utils/parser.hpp"

#include <iostream>


int main(int argc, char* argv[]) {
    if (argc < 5) {
        std::cerr << "Usage: " << argv[0] << " <config_path> <ipv4_address> <port> <max_connection_count>" << std::endl;
        return 1;
    }

    auto [ config_path, address, port, max_connect ] = utils::parse_args(argv);
    auto [ cpu_limit, thread_limit, doc_root ] = utils::parse_config(config_path);

    std::cout << cpu_limit << ' ' << thread_limit << ' ' << doc_root;
}