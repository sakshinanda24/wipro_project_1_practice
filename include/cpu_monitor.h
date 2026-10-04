#ifndef CPU_MONITOR_H
#define CPU_MONITOR_H

#include <string>

class CPUMonitor {
public:
    CPUMonitor();
    ~CPUMonitor();

    double getCPUUsage();
    bool isCPUOverloaded(double threshold);
    std::string getCPUInfo();

private:
    double calculateCPUUsage();
    long readCPUStats();
};

#endif // CPU_MONITOR_H
