#include "../include/config.hpp"
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>

Options load_config(const path &config_file_name) {
    Options options;
    std::unordered_map<string, string> config_map;
    std::ifstream file(config_file_name);
    std::cout << config_file_name << std::endl;

    if(!file.is_open()) {
        throw std::runtime_error("unable to load configuration file");
    }

    string line;
    while(std::getline(file, line)) {
        std::istringstream line_stream(line);
        string key, value;
        if(std::getline(line_stream, key, '=') && std::getline(line_stream, value)) {
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);
            config_map[key] = value;
        }
    }
    try {
        options.port = std::stoi(config_map.at("port"));
    } catch (...) {
        throw  std::runtime_error("Invalid or missing port in configuration file");
    }

    try {
        options.root = config_map.at("root");
    } catch (...) {
        throw std::runtime_error("Invalid or missing root directory in configuration file");
    }

    try {
        string log_value =  config_map.at("log");
        if (log_value == "true") {
            options.log = true;
        } else if(log_value == "false") {
            options.log = false;
        } else {
            std::cout << "shoulnnt be something else\n";
        }
    } catch(...) {
        options.log = false;
    }

    if(options.log) {
        try {
            options.directory_for_log = config_map.at("log_path");
        } catch(...) {
            options.directory_for_log = "/var/log/mini_apache.log";
        }
    }
    return options;
}
