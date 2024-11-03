#pragma once

#include <string>
#include <filesystem>
#include <unordered_map>
#include "./http_server.hpp"

namespace utils {
    enum class FileReadStatus {
        Success, 
        FileNotFound, 
        PermissionDenied, 
        ReadError,
    };

    std::string get_mime_type(const std::string& file_name);
    FileReadStatus read_file(const std::filesystem::path& file_path, std::string& file_content);
}
