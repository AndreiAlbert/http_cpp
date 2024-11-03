#include "../include/utils.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>

namespace fs = std::filesystem;

namespace utils {

    std::string get_mime_type(const std::string& file_name) {
        size_t pos = file_name.find_last_of('.');
        std::string extension = (pos != std::string::npos) ? file_name.substr(pos + 1) : "";
        static std::unordered_map<std::string, std::string> mime_types = {
            {"html", "text/html"},
            {"json", "application/json"},
            {"js", "application/javascript"},
            {"css", "text/css"},
            {"jpg", "image/jpeg"},
            {"png", "image/png"},
            {"txt", "text/plain"}
        };

        auto it = mime_types.find(extension);
        if (it != mime_types.end()) {
            return it->second;
        }
        return "application/octet-stream";
    }

    FileReadStatus read_file(const fs::path& file_path, std::string& file_content) {
        if (!fs::exists(file_path)) {
            std::cerr << "File not found: " << file_path << std::endl;
            return FileReadStatus::FileNotFound;
        }

        if (!fs::is_regular_file(file_path)) {
            std::cerr << "Not a regular file: " << file_path << std::endl;
            return FileReadStatus::FileNotFound;
        }

        std::ifstream file(file_path, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Permission denied to open file: " << file_path << std::endl;
            return FileReadStatus::PermissionDenied;
        }

        std::ostringstream file_stream;
        file_stream << file.rdbuf();
        if (file.fail()) {
            std::cerr << "Error reading file: " << file_path << std::endl;
            return FileReadStatus::ReadError;
        }
        file_content = file_stream.str();
        return FileReadStatus::Success;
    }

} // namespace utils
