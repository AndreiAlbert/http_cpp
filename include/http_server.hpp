#pragma once

#include <filesystem>
#include <string>
#include <unordered_map>

using std::string;
namespace fs = std::filesystem;

struct HttpRequest {
    string method;
    string url;
    string http_version;
    std::unordered_map<string, string> headers;
    std::string body;
};


class HttpServer {
public:
    HttpServer(int port, const fs::path& root_dir);
    ~HttpServer();
    bool start();
private:
    int port_;
    int server_fd_;
    fs::path root_dir;
    void send_response(int client_fd, int status_code, const string& content, const string& content_type);
    void handle_request(int client_fd);
    static HttpRequest parse_request(const std::string& request_text);
    fs::path map_request_to_file(const std::string& request_url);
};
