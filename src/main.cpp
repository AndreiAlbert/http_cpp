#include "../include/http_server.hpp"
#include "../include/config.hpp"
#include "../include/utils.hpp"
#include <cstdlib>
#include <exception>
#include <filesystem>
#include <iostream>

int main() {
    Options options;
    try {
        options = load_config("./config.ini");
    } catch (const std::exception& e) {
        std::cerr << "Error loading configuration file: " << e.what() << std::endl;
        exit(1);
    }
    auto path = utils::expand_tilde(options.root);
    HttpServer server(options.port, path);
    if(!server.start()) {
        std::cerr << "Could not start server" << std::endl;
        exit(1);
    }
    return 0;
}
