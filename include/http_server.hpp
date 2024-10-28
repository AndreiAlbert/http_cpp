#pragma once

#include <string>
#include <unordered_map>

using std::string;

struct HttpRequest {
    string method;
    string path;
    string http_version;
    std::unordered_map<string, string> headers;
    std::string body;
};

HttpRequest parse_request(const std::string& request_text);

class HttpServer {
public:
    HttpServer(int port);
    ~HttpServer();
    bool start();
private:
    int port_;
    int server_fd_;
    void handle_request(int client_fd);
};
