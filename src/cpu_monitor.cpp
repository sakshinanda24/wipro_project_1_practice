#include "cpu_monitor.h"
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unistd.h>

CPUMonitor::CPUMonitor() {}

CPUMonitor::~CPUMonitor() {}

double CPUMonitor::getCPUUsage() {
    return calculateCPUUsage();
}

bool CPUMonitor::isCPUOverloaded(double threshold) {
    double usage = getCPUUsage();
    return usage > threshold;
}

std::string CPUMonitor::getCPUInfo() {
    std::ifstream file("/proc/cpuinfo");
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

double CPUMonitor::calculateCPUUsage() {
    static long long prevIdle = 0;
    static long long prevTotal = 0;

    std::ifstream file("/proc/stat");
    std::string line;
    std::getline(file, line);
    std::istringstream iss(line);

    std::string cpu;
    long long user, nice, system, idle, iowait, irq, softirq, steal;
    
    iss >> cpu >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal;

    long long total = user + nice + system + idle + iowait + irq + softirq + steal;
    long long idleTotal = idle + iowait;

    long long diffIdle = idleTotal - prevIdle;
    long long diffTotal = total - prevTotal;

    double cpuUsage = 0.0;
    if (diffTotal > 0) {
        cpuUsage = (double)(diffTotal - diffIdle) / diffTotal * 100.0;
    }

    prevIdle = idleTotal;
    prevTotal = total;

    return cpuUsage;
}
