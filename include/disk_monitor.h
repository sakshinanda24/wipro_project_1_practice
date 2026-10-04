#ifndef DISK_MONITOR_H
#define DISK_MONITOR_H

#include <string>

class DiskMonitor {
public:
    DiskMonitor();
    ~DiskMonitor();

    double getDiskUsagePercent(const std::string& path = "/");
    long getDiskFreeGB(const std::string& path = "/");
    bool isDiskOverloaded(double threshold, const std::string& path = "/");
    std::string getDiskInfo(const std::string& path = "/");

private:
    double calculateDiskUsage(const std::string& path);
};

#endif // DISK_MONITOR_H
