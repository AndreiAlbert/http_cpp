#include "../include/http_server.hpp"
#include "../include/logger.hpp"
#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sstream>
#include <unordered_map>
#include "../include/utils.hpp"

#define BUFFER_SIZE 1024

using std::istringstream;
using utils::FileReadStatus;
using utils::read_file;
using utils::get_mime_type;
using utils::generate_directory_page;

Logger logger("server.log");

HttpServer::HttpServer(int port, const fs::path& root_dir) : port_(port), server_fd_(-1), root_dir(root_dir) {}

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
    logger.log(Logger::LogLevel::INFO, "Handling new request");
    std::string request_data;  
    ssize_t bytes_received;
    size_t content_length = 0;
    bool headers_complete = false;
    while (true) {
        char buffer[BUFFER_SIZE];
        bytes_received = recv(client_fd, buffer, BUFFER_SIZE, 0);
        if (bytes_received < 0) {
            logger.log(Logger::LogLevel::ERROR, "Error receiving data from client");
            std::cerr << "Error receiving data." << std::endl;
            return;
        } else if (bytes_received == 0) {
            break;
        }
        request_data.append(buffer, bytes_received);
        if (!headers_complete) {
            size_t pos = request_data.find("\r\n\r\n");
            if (pos != std::string::npos) {
                headers_complete = true;
                std::istringstream header_stream(request_data.substr(0, pos + 4));
                std::string line;
                while (std::getline(header_stream, line) && line != "\r") {
                    if (line.back() == '\r') line.pop_back(); 
                    if (line.find("Content-Length:") == 0) {
                        content_length = std::stoi(line.substr(15));
                    }
                }
                if (content_length == 0) {
                    break;
                }
            }
        }
        if (headers_complete && content_length > 0) {
            size_t header_end_pos = request_data.find("\r\n\r\n") + 4;
            size_t body_length = request_data.size() - header_end_pos;
            if (body_length >= content_length) {
                break;
            }
        }
    }
    HttpRequest request = parse_request(request_data);
    logger.log(Logger::LogLevel::INFO, "Parsed request URL: " + request.url);
    auto full_path = this->map_request_to_file(request.url);
    if (fs::exists(full_path) && fs::is_directory(full_path)) {
        logger.log(Logger::LogLevel::INFO, "Request is for a directory: " + full_path.string());
        auto html_body =  generate_directory_page(full_path, request.url);
        send_response(client_fd, 200, html_body, "text/html");
        return;
    }
    string file_content;
    FileReadStatus status = read_file(full_path, file_content);
    auto content_type = get_mime_type(full_path);
    logger.log(Logger::LogLevel::INFO, "Request is for a file: " + full_path.string());
    switch (status) {
        case FileReadStatus::FileNotFound:
            logger.log(Logger::LogLevel::ERROR, "File not Found: " + full_path.string());
            send_response(client_fd, 404, "404 Not Found", content_type);
        break;
        case FileReadStatus::PermissionDenied:
            logger.log(Logger::LogLevel::ERROR, "Permission denied: " + full_path.string());
            send_response(client_fd, 403, "403 Forbidden", content_type);
        break;
        case FileReadStatus::ReadError:
            logger.log(Logger::LogLevel::ERROR, "Error reading file: " + full_path.string());
            send_response(client_fd, 500, "500 Server Error", content_type);
        break;
        case FileReadStatus::Success:
            logger.log(Logger::LogLevel::ERROR, "Successfulyl read file: " + full_path.string());
            send_response(client_fd, 200, file_content, content_type);
        break;
    }
}

void HttpServer::send_response(int client_fd, int status_code, const string& content, const string& content_type) {
    std::unordered_map<int, string> status_map = {
        {200, "OK"}, {404, "Not Found"}, {403, "Forbidden"}, {500, "Internal Server Error"}
    };
    string status_text;
    if (status_map.find(status_code) != status_map.end()) {
        status_text = status_map[status_code];
    } else {
        status_text = "Unkown status";
    }
    std::ostringstream response_stream;
    response_stream << "HTTP/1.1 " << status_code << " " << status_text << "\r\n"
                    << "Content-Type: " << content_type << "\r\n"
                    << "Content-Length: " << content.size() << "\r\n"
                    << "Connection: closer\r\n"
                    << "\r\n"
                    << content;
    string response = response_stream.str();
    send(client_fd, response.c_str(), response.size(), 0);
}

HttpServer::~HttpServer() {
    if(server_fd_ > 0) {
        close(server_fd_);
    }
}

HttpRequest HttpServer::parse_request(const std::string &request_text) {
    HttpRequest request;
    istringstream request_stream(request_text);
    std::string request_line;

    if(std::getline(request_stream, request_line)) {
        std::istringstream line_stream(request_line);
        line_stream >> request.method >> request.url >> request.http_version;
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
    return request;
}

fs::path HttpServer::map_request_to_file(const std::string& request_url) {
    if(request_url == "/")  {
        return this->root_dir;
    }
    std::string clean_path;
    if(!request_url.empty() && request_url[0] == '/') {
        clean_path = request_url.substr(1);
    }
    return this->root_dir / clean_path;
}
