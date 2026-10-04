#include "memory_monitor.h"
#include <fstream>
#include <sstream>
#include <string>

MemoryMonitor::MemoryMonitor() {}

MemoryMonitor::~MemoryMonitor() {}

double MemoryMonitor::getMemoryUsagePercent() {
    long total = getTotalMemoryKB();
    long available = getAvailableMemoryKB();
    
    if (total == 0) return 0.0;
    
    long used = total - available;
    return (double)used / total * 100.0;
}

long MemoryMonitor::getMemoryUsageMB() {
    long total = getTotalMemoryKB();
    long available = getAvailableMemoryKB();
    return (total - available) / 1024;
}

bool MemoryMonitor::isMemoryOverloaded(double threshold) {
    double usage = getMemoryUsagePercent();
    return usage > threshold;
}

std::string MemoryMonitor::getMemoryInfo() {
    std::ifstream file("/proc/meminfo");
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

long MemoryMonitor::getTotalMemoryKB() {
    std::ifstream file("/proc/meminfo");
    std::string line;
    
    while (std::getline(file, line)) {
        if (line.find("MemTotal:") == 0) {
            std::istringstream iss(line);
            std::string label;
            long value;
            iss >> label >> value;
            return value;
        }
    }
    return 0;
}

long MemoryMonitor::getAvailableMemoryKB() {
    std::ifstream file("/proc/meminfo");
    std::string line;
    
    while (std::getline(file, line)) {
        if (line.find("MemAvailable:") == 0) {
            std::istringstream iss(line);
            std::string label;
            long value;
            iss >> label >> value;
            return value;
        }
    }
    return 0;
}
