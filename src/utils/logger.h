#pragma once

#include <string>
#include <iostream>

class Logger
{
public:
    enum Level {
        DEBUG,
        INFO,
        WARNING,
        ERROR
    };
    
    static void setLogLevel(Level level) { s_logLevel = level; }
    
    static void debug(const std::string& message) { log(DEBUG, message); }
    static void info(const std::string& message) { log(INFO, message); }
    static void warning(const std::string& message) { log(WARNING, message); }
    static void error(const std::string& message) { log(ERROR, message); }
    
private:
    static void log(Level level, const std::string& message);
    static std::string getLevelString(Level level);
    
    static Level s_logLevel;
};
