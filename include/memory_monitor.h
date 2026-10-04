#ifndef MEMORY_MONITOR_H
#define MEMORY_MONITOR_H

#include <string>

class MemoryMonitor {
public:
    MemoryMonitor();
    ~MemoryMonitor();

    double getMemoryUsagePercent();
    long getMemoryUsageMB();
    bool isMemoryOverloaded(double threshold);
    std::string getMemoryInfo();

private:
    long getTotalMemoryKB();
    long getAvailableMemoryKB();
};

#endif // MEMORY_MONITOR_H
