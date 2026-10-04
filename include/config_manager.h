#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <string>
#include <map>
#include <fstream>
#include <sstream>
#include <iostream>

struct MonitorConfig {
    int monitoringIntervalSeconds;
    double cpuUsagePercent;
    double memoryUsagePercent;
    double diskUsagePercent;
    double temperatureCriticalCelsius;
    double temperatureWarningCelsius;
    std::string networkInterface;
    int networkCheckIntervalSeconds;
    std::string criticalService;
    bool restartOnFailure;
    double killProcessThresholdPercent;
    int maxRestartAttempts;
    int restartDelaySeconds;
    std::string logLevel;
    std::string logFile;
    int maxLogSizeMB;
    int maxLogFiles;
};

class ConfigManager {
public:
    static ConfigManager& getInstance();
    bool loadConfig(const std::string& configPath);
    MonitorConfig getConfig() const;
    bool saveConfig(const std::string& configPath);

private:
    ConfigManager();
    ~ConfigManager();
    ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;

    std::string trim(const std::string& str);
    int parseInt(const std::string& value, int defaultValue);
    double parseDouble(const std::string& value, double defaultValue);
    bool parseBool(const std::string& value, bool defaultValue);

    MonitorConfig config_;
    bool configLoaded_;
};

#endif // CONFIG_MANAGER_H
