#include "../include/logger.hpp"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>

Logger::Logger(const std::string& log_file_path) {
    log_file_.open(log_file_path, std::ios::app);
    if (!log_file_.is_open()) {
        std::cerr << "Failed to open log file: " << log_file_path << std::endl;
    }
}

Logger::~Logger() {
    if (log_file_.is_open()) {
        log_file_.close();
    }
}

void Logger::log(LogLevel level, const std::string& message) {
    std::string timestamp = get_timestamp();
    std::string log_entry = "[" + timestamp + "]" + level_to_string(level) + "]" + message;
    std::cout << log_entry << std::endl;
    if(log_file_.is_open()) {
        log_file_ << log_entry << std::endl;
    }
}

std::string Logger::get_timestamp() const {
    using std::chrono::system_clock;
    auto now = system_clock::now();
    auto time = system_clock::to_time_t(now);
    std::tm tm;
    localtime_r(&time, &tm);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

std::string Logger::level_to_string(LogLevel level) const {
    switch (level) {
        case LogLevel::INFO: return "INFO";
        case LogLevel::ERROR: return "ERROR";
        default: return "UNKOWN";
    }
}
