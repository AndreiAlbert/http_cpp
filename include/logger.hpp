#pragma once

#include <fstream>
#include <string>

class Logger {
public:
    enum class LogLevel {
        INFO, 
        ERROR,
    };

    Logger(const std::string& log_file_path);
    ~Logger();

    void log(LogLevel level, const std::string& message);

private:
    std::ofstream log_file_;
    std::string get_timestamp() const;
    std::string level_to_string(LogLevel level) const;
};
