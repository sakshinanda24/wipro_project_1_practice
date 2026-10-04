#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <fstream>
#include <mutex>
#include <chrono>

enum class LogLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    CRITICAL
};

class Logger {
public:
    static Logger& getInstance();
    void log(LogLevel level, const std::string& message);
    void setLogLevel(LogLevel level);
    void setLogFilePath(const std::string& path);
    void rotateLogs();

private:
    Logger();
    ~Logger();
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    std::string levelToString(LogLevel level);
    std::string getCurrentTimestamp();
    void writeLog(const std::string& message);

    std::ofstream logFile_;
    std::mutex mutex_;
    LogLevel currentLevel_;
    std::string logFilePath_;
    static const size_t MAX_LOG_SIZE = 10 * 1024 * 1024; // 10MB
};

#endif // LOGGER_H
