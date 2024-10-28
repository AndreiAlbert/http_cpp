#include "../include/http_server.hpp"
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sstream>

using std::istringstream;

HttpServer::HttpServer(int port) : port_(port), server_fd_(-1) {}

bool HttpServer::start() {
    server_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd_ < 0) {
        std::cerr << "Failed to create socket" << std::endl;
        return false;
    }
    sockaddr_in server_addr {};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port_);

    if (bind(server_fd_, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Bind failed." << std::endl;
        return false;
    }
    
    if(listen(server_fd_, 10) < 0) {
        std::cerr << "Listen Failed" << std::endl;
        return false;
    }

    std::cout << "Server started on port " << port_ << std::endl;

    while(true) {
        sockaddr_in client_addr {};
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(server_fd_, (struct sockaddr*)&client_addr, &client_len);
        if(client_fd < 0) {
            std::cerr << "Failed to accept connection" << std::endl;
            continue;
        }
        handle_request(client_fd);
        close(client_fd);
    }
}


void HttpServer::handle_request(int client_fd) {
    const int buffer_size = 2048;
    char buffer[buffer_size];
    ssize_t bytes_received = recv(client_fd, buffer, buffer_size - 1, 0); 
    if(bytes_received < 0) {
        std::cerr << "Failed to receive data." << std::endl;
        return;
    }

    parse_request(buffer);
    buffer[bytes_received] = '\0';
    std::string response = 
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: 13\r\n"
        "\r\n"
        "Hello, World!";
    send(client_fd, response.c_str(), response.size(), 0);
}

HttpServer::~HttpServer() {
    if(server_fd_ > 0) {
        close(server_fd_);
    }
}

void debug_http_req(HttpRequest r) {
    std::cout << "LOGGING\n";
    std::cout << "Method: " << r.method << std::endl;
    std::cout << "Path: " << r.path << std::endl;
    std::cout << "http_version: " << r.http_version << std::endl;
    std::cout << "body: " << r.body << std::endl;
    std::cout << "Headers: \n";
    for(const auto& it: r.headers) {
        std::cout << it.first << " " << it.second << "\n";
    }
}



HttpRequest parse_request(const std::string &request_text) {
    HttpRequest request;
    istringstream request_stream(request_text);
    std::string request_line;

    if(std::getline(request_stream, request_line)) {
        std::istringstream line_stream(request_line);
        line_stream >> request.method >> request.path >> request.http_version;
    }
    
    std::string header_line;
    while(std::getline(request_stream, header_line) && header_line != "\r") {
        if(header_line.back() == '\r') {
            header_line.pop_back();
        }
        auto colon_pos = header_line.find(':');
        if(colon_pos != string::npos) {
            std::string header_name = header_line.substr(0, colon_pos);
            std::string header_value = header_line.substr(colon_pos + 1);

            header_name.erase(header_name.find_last_not_of(" \t") + 1);
            header_value.erase(0, header_value.find_first_not_of(" \t"));

            request.headers[header_name] = header_value;
        }
    }

    auto content_length_it = request.headers.find("Content-Length");
    if(content_length_it != request.headers.end()) {
        size_t content_length = std::stoi(content_length_it->second);
        request.body.resize(content_length);
        request_stream.read(&request.body[0], content_length);
    }
    debug_http_req(request);
    return request;
}
