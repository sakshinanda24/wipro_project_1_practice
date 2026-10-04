#ifndef TEMPERATURE_MONITOR_H
#define TEMPERATURE_MONITOR_H

#include <string>
#include <vector>

class TemperatureMonitor {
public:
    TemperatureMonitor();
    ~TemperatureMonitor();

    double getCPUTemperature();
    double getSystemTemperature();
    bool isTemperatureCritical(double threshold);
    bool isTemperatureWarning(double threshold);
    std::vector<std::string> getTemperatureSensors();

private:
    std::string readTemperatureFile(const std::string& path);
    std::vector<std::string> findTemperatureSensors();
};

#endif // TEMPERATURE_MONITOR_H
