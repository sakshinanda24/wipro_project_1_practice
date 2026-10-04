#include "logger.h"
#include <iostream>
#include <iomanip>
#include <ctime>
#include <chrono>
#include <sys/stat.h>

Logger::Logger() : currentLevel_(LogLevel::INFO), logFilePath_("/var/log/device_health_monitor.log") {
    // Ensure log directory exists
    std::string dir = "/var/log";
    mkdir(dir.c_str(), 0755);
    
    logFile_.open(logFilePath_, std::ios::app);
}

Logger::~Logger() {
    if (logFile_.is_open()) {
        logFile_.close();
    }
}

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

void Logger::log(LogLevel level, const std::string& message) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (level < currentLevel_) {
        return;
    }

    std::string timestamp = getCurrentTimestamp();
    std::string levelStr = levelToString(level);
    std::string fullMessage = "[" + timestamp + "] [" + levelStr + "] " + message;

    writeLog(fullMessage);
    std::cout << fullMessage << std::endl;
}

void Logger::setLogLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(mutex_);
    currentLevel_ = level;
}

void Logger::setLogFilePath(const std::string& path) {
    std::lock_guard<std::mutex> lock(mutex_);
    logFilePath_ = path;
    
    if (logFile_.is_open()) {
        logFile_.close();
    }
    logFile_.open(logFilePath_, std::ios::app);
}

void Logger::rotateLogs() {
    if (logFile_.is_open()) {
        logFile_.close();
    }

    // Simple rotation: rename current log to .1
    std::string rotatedPath = logFilePath_ + ".1";
    rename(logFilePath_.c_str(), rotatedPath.c_str());
    
    logFile_.open(logFilePath_, std::ios::app);
}

std::string Logger::levelToString(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::INFO: return "INFO";
        case LogLevel::WARNING: return "WARNING";
        case LogLevel::ERROR: return "ERROR";
        case LogLevel::CRITICAL: return "CRITICAL";
        default: return "UNKNOWN";
    }
}

std::string Logger::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;

    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S")
       << '.' << std::setfill('0') << std::setw(3) << ms.count();
    return ss.str();
}

void Logger::writeLog(const std::string& message) {
    if (!logFile_.is_open()) {
        logFile_.open(logFilePath_, std::ios::app);
    }

    logFile_ << message << std::endl;
    logFile_.flush();

    // Check if rotation is needed
    logFile_.seekp(0, std::ios::end);
    if (logFile_.tellp() > MAX_LOG_SIZE) {
        rotateLogs();
    }
}
