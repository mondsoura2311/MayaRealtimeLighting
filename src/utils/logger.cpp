#include "logger.h"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>

Logger::Level Logger::s_logLevel = Logger::INFO;

std::string Logger::getLevelString(Level level)
{
    switch (level) {
        case DEBUG: return "DEBUG";
        case INFO: return "INFO";
        case WARNING: return "WARNING";
        case ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}

void Logger::log(Level level, const std::string& message)
{
    if (level < s_logLevel) return;
    
    // Get current time
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%H:%M:%S")
       << '.' << std::setfill('0') << std::setw(3) << ms.count();
    
    std::string levelStr = getLevelString(level);
    std::string logMessage = "[" + ss.str() + "] [" + levelStr + "] " + message;
    
    // Output to console
    if (level == ERROR) {
        std::cerr << logMessage << std::endl;
    } else {
        std::cout << logMessage << std::endl;
    }
}
