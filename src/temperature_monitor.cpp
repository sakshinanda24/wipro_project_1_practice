#include "temperature_monitor.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <dirent.h>
#include <algorithm>
#include <cstring>

TemperatureMonitor::TemperatureMonitor() {}

TemperatureMonitor::~TemperatureMonitor() {}

double TemperatureMonitor::getCPUTemperature() {
    return getSystemTemperature();
}

double TemperatureMonitor::getSystemTemperature() {
    std::vector<std::string> sensors = getTemperatureSensors();
    
    if (sensors.empty()) {
        // Fallback: try common thermal zone
        std::ifstream file("/sys/class/thermal/thermal_zone0/temp");
        if (file.is_open()) {
            std::string content;
            std::getline(file, content);
            return std::stod(content) / 1000.0;
        }
        return 0.0;
    }

    double totalTemp = 0.0;
    int count = 0;

    for (const auto& sensor : sensors) {
        std::string tempStr = readTemperatureFile(sensor);
        double temp = std::stod(tempStr) / 1000.0;
        if (temp > 0) {
            totalTemp += temp;
            count++;
        }
    }

    return count > 0 ? totalTemp / count : 0.0;
}

bool TemperatureMonitor::isTemperatureCritical(double threshold) {
    double temp = getSystemTemperature();
    return temp >= threshold;
}

bool TemperatureMonitor::isTemperatureWarning(double threshold) {
    double temp = getSystemTemperature();
    return temp >= threshold;
}

std::vector<std::string> TemperatureMonitor::getTemperatureSensors() {
    return findTemperatureSensors();
}

std::string TemperatureMonitor::readTemperatureFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        return "0";
    }
    std::string content;
    std::getline(file, content);
    return content;
}

std::vector<std::string> TemperatureMonitor::findTemperatureSensors() {
    std::vector<std::string> sensors;
    
    DIR* dir = opendir("/sys/class/thermal");
    if (!dir) return sensors;

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        if (entry->d_type == DT_DIR && 
            strncmp(entry->d_name, "thermal_zone", 12) == 0) {
            std::string path = std::string("/sys/class/thermal/") + 
                             entry->d_name + "/temp";
            sensors.push_back(path);
        }
    }
    closedir(dir);

    return sensors;
}
