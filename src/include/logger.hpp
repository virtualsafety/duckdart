#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <filesystem>

class Logger {
public:
    enum LogLevel {
        DEBUG,
        INFO,
        WARNING,
        ERROR
    };

    Logger(const std::string& filename, LogLevel level = INFO) 
        : m_filename(filename), m_level(level) {
        m_file.open(filename, std::ios::app);
    }

    ~Logger() {
        if (m_file.is_open()) {
            m_file.close();
        }
    }

    void setLogLevel(LogLevel level) {
        m_level = level;
    }

    void debug(const std::string& message, const char* file, const char* function, int line) {
        log(DEBUG, "DEBUG", message, file, function, line);
    }

    void info(const std::string& message, const char* file, const char* function, int line) {
        log(INFO, "INFO", message, file, function, line);
    }

    void warning(const std::string& message, const char* file, const char* function, int line) {
        log(WARNING, "WARNING", message, file, function, line);
    }

    void error(const std::string& message, const char* file, const char* function, int line) {
        log(ERROR, "ERROR", message, file, function, line);
    }

private:
    void log(LogLevel messageLevel, const std::string& levelString, const std::string& message, 
             const char* file, const char* function, int line) {
        if (messageLevel >= m_level) {
            std::time_t now = std::time(nullptr);
            std::tm* localTime = std::localtime(&now);

            std::filesystem::path filePath(file);
            std::string fileName = filePath.filename().string();

            std::stringstream ss;
            ss << "[" << std::put_time(localTime, "%Y-%m-%d %H:%M:%S") << "] "
               << "[" << levelString << "] "
               << "[" << fileName << ":" << line << "," << function << "()] "
               << message;

            m_file << ss.str() << std::endl;

            // Also print to console
            //std::cout << ss.str() << std::endl;
        }
    }

    std::string m_filename;
    std::ofstream m_file;
    LogLevel m_level;
};

extern Logger g_logger;

#define LOG_DEBUG(message) g_logger.debug(message, __FILE__, __FUNCTION__, __LINE__)
#define LOG_INFO(message) g_logger.info(message, __FILE__, __FUNCTION__, __LINE__)
#define LOG_WARNING(message) g_logger.warning(message, __FILE__, __FUNCTION__, __LINE__)
#define LOG_ERROR(message) g_logger.error(message, __FILE__, __FUNCTION__, __LINE__)



