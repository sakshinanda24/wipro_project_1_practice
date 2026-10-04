#include "config_manager.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <sys/stat.h>

ConfigManager::ConfigManager() : configLoaded_(false) {
    // Set default values
    config_.monitoringIntervalSeconds = 10;
    config_.cpuUsagePercent = 80.0;
    config_.memoryUsagePercent = 85.0;
    config_.diskUsagePercent = 90.0;
    config_.temperatureCriticalCelsius = 85.0;
    config_.temperatureWarningCelsius = 75.0;
    config_.networkInterface = "eth0";
    config_.networkCheckIntervalSeconds = 30;
    config_.criticalService = "sshd";
    config_.restartOnFailure = true;
    config_.killProcessThresholdPercent = 95.0;
    config_.maxRestartAttempts = 3;
    config_.restartDelaySeconds = 5;
    config_.logLevel = "INFO";
    config_.logFile = "/var/log/device_health_monitor.log";
    config_.maxLogSizeMB = 10;
    config_.maxLogFiles = 5;
}

ConfigManager::~ConfigManager() {}

ConfigManager& ConfigManager::getInstance() {
    static ConfigManager instance;
    return instance;
}

bool ConfigManager::loadConfig(const std::string& configPath) {
    std::ifstream file(configPath);
    if (!file.is_open()) {
        std::cerr << "Warning: Could not open config file: " << configPath << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') continue;

        size_t pos = line.find(':');
        if (pos == std::string::npos) continue;

        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));

        // Remove quotes from value
        if (!value.empty() && value.front() == '"') {
            value = value.substr(1, value.length() - 2);
        }

        // Parse key-value pairs
        if (key == "monitoring_interval_seconds") {
            config_.monitoringIntervalSeconds = parseInt(value, 10);
        } else if (key == "cpu_usage_percent") {
            config_.cpuUsagePercent = parseDouble(value, 80.0);
        } else if (key == "memory_usage_percent") {
            config_.memoryUsagePercent = parseDouble(value, 85.0);
        } else if (key == "disk_usage_percent") {
            config_.diskUsagePercent = parseDouble(value, 90.0);
        } else if (key == "temperature_critical_celsius") {
            config_.temperatureCriticalCelsius = parseDouble(value, 85.0);
        } else if (key == "temperature_warning_celsius") {
            config_.temperatureWarningCelsius = parseDouble(value, 75.0);
        } else if (key == "interface") {
            config_.networkInterface = value;
        } else if (key == "check_interval_seconds") {
            config_.networkCheckIntervalSeconds = parseInt(value, 30);
        } else if (key == "critical_service") {
            config_.criticalService = value;
        } else if (key == "restart_on_failure") {
            config_.restartOnFailure = parseBool(value, true);
        } else if (key == "kill_process_threshold_percent") {
            config_.killProcessThresholdPercent = parseDouble(value, 95.0);
        } else if (key == "max_restart_attempts") {
            config_.maxRestartAttempts = parseInt(value, 3);
        } else if (key == "restart_delay_seconds") {
            config_.restartDelaySeconds = parseInt(value, 5);
        } else if (key == "level") {
            config_.logLevel = value;
        } else if (key == "file") {
            config_.logFile = value;
        } else if (key == "max_size_mb") {
            config_.maxLogSizeMB = parseInt(value, 10);
        } else if (key == "max_files") {
            config_.maxLogFiles = parseInt(value, 5);
        }
    }

    configLoaded_ = true;
    return true;
}

MonitorConfig ConfigManager::getConfig() const {
    return config_;
}

bool ConfigManager::saveConfig(const std::string& configPath) {
    std::ofstream file(configPath);
    if (!file.is_open()) {
        return false;
    }

    file << "monitoring_interval_seconds: " << config_.monitoringIntervalSeconds << "\n";
    file << "cpu_usage_percent: " << config_.cpuUsagePercent << "\n";
    file << "memory_usage_percent: " << config_.memoryUsagePercent << "\n";
    file << "disk_usage_percent: " << config_.diskUsagePercent << "\n";
    file << "temperature_critical_celsius: " << config_.temperatureCriticalCelsius << "\n";
    file << "temperature_warning_celsius: " << config_.temperatureWarningCelsius << "\n";
    file << "interface: " << config_.networkInterface << "\n";
    file << "check_interval_seconds: " << config_.networkCheckIntervalSeconds << "\n";
    file << "critical_service: " << config_.criticalService << "\n";
    file << "restart_on_failure: " << (config_.restartOnFailure ? "true" : "false") << "\n";
    file << "kill_process_threshold_percent: " << config_.killProcessThresholdPercent << "\n";
    file << "max_restart_attempts: " << config_.maxRestartAttempts << "\n";
    file << "restart_delay_seconds: " << config_.restartDelaySeconds << "\n";
    file << "level: " << config_.logLevel << "\n";
    file << "file: " << config_.logFile << "\n";
    file << "max_size_mb: " << config_.maxLogSizeMB << "\n";
    file << "max_files: " << config_.maxLogFiles << "\n";

    return true;
}

std::string ConfigManager::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, last - first + 1);
}

int ConfigManager::parseInt(const std::string& value, int defaultValue) {
    try {
        return std::stoi(value);
    } catch (...) {
        return defaultValue;
    }
}

double ConfigManager::parseDouble(const std::string& value, double defaultValue) {
    try {
        return std::stod(value);
    } catch (...) {
        return defaultValue;
    }
}

bool ConfigManager::parseBool(const std::string& value, bool defaultValue) {
    if (value == "true" || value == "1" || value == "yes") return true;
    if (value == "false" || value == "0" || value == "no") return false;
    return defaultValue;
}
