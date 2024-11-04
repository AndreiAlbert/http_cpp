#pragma once

#include <string>
#include <filesystem>

namespace fs = std::filesystem;

namespace utils {
    enum class FileReadStatus {
        Success, 
        FileNotFound, 
        PermissionDenied, 
        ReadError,
    };

    std::string get_mime_type(const std::string& file_name);
    FileReadStatus read_file(const std::filesystem::path& file_path, std::string& file_content);
    std::string generate_directory_page(const fs::path& dir_path, const std::string& req_url);
}
