#include "disk_monitor.h"
#include <sys/statvfs.h>
#include <string>

DiskMonitor::DiskMonitor() {}

DiskMonitor::~DiskMonitor() {}

double DiskMonitor::getDiskUsagePercent(const std::string& path) {
    return calculateDiskUsage(path);
}

long DiskMonitor::getDiskFreeGB(const std::string& path) {
    struct statvfs stat;
    if (statvfs(path.c_str(), &stat) != 0) {
        return 0;
    }
    return (stat.f_bavail * stat.f_frsize) / (1024 * 1024 * 1024);
}

bool DiskMonitor::isDiskOverloaded(double threshold, const std::string& path) {
    double usage = getDiskUsagePercent(path);
    return usage > threshold;
}

std::string DiskMonitor::getDiskInfo(const std::string& path) {
    struct statvfs stat;
    if (statvfs(path.c_str(), &stat) != 0) {
        return "Unable to get disk info";
    }

    double totalGB = (double)stat.f_blocks * stat.f_frsize / (1024 * 1024 * 1024);
    double freeGB = (double)stat.f_bfree * stat.f_frsize / (1024 * 1024 * 1024);
    double usedGB = totalGB - freeGB;
    double usagePercent = usedGB / totalGB * 100.0;

    char buffer[256];
    snprintf(buffer, sizeof(buffer), 
             "Total: %.2f GB, Used: %.2f GB, Free: %.2f GB, Usage: %.2f%%",
             totalGB, usedGB, freeGB, usagePercent);
    return std::string(buffer);
}

double DiskMonitor::calculateDiskUsage(const std::string& path) {
    struct statvfs stat;
    if (statvfs(path.c_str(), &stat) != 0) {
        return 0.0;
    }

    double total = (double)stat.f_blocks * stat.f_frsize;
    double freeSpace = (double)stat.f_bfree * stat.f_frsize;
    double used = total - freeSpace;

    return used / total * 100.0;
}
