#include "service_monitor.h"
#include <cstdlib>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

ServiceMonitor::ServiceMonitor() {}

ServiceMonitor::~ServiceMonitor() {}

bool ServiceMonitor::isServiceRunning(const std::string& serviceName) {
    std::string command = "pgrep -x " + serviceName + " > /dev/null 2>&1";
    return executeCommand(command) == 0;
}

bool ServiceMonitor::startService(const std::string& serviceName) {
    std::string command = "systemctl start " + serviceName + " 2>&1";
    return executeCommand(command) == 0;
}

bool ServiceMonitor::stopService(const std::string& serviceName) {
    std::string command = "systemctl stop " + serviceName + " 2>&1";
    return executeCommand(command) == 0;
}

bool ServiceMonitor::restartService(const std::string& serviceName) {
    std::string command = "systemctl restart " + serviceName + " 2>&1";
    return executeCommand(command) == 0;
}

int ServiceMonitor::getServicePID(const std::string& serviceName) {
    std::string command = "pgrep -x " + serviceName;
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) return -1;

    int pid = -1;
    char buffer[128];
    if (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        pid = std::stoi(std::string(buffer));
    }
    pclose(pipe);
    return pid;
}

std::string ServiceMonitor::getServiceStatus(const std::string& serviceName) {
    std::string command = "systemctl status " + serviceName + " 2>&1";
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) return "Unable to get status";

    std::string result;
    char buffer[256];
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result += buffer;
    }
    pclose(pipe);
    return result;
}

int ServiceMonitor::executeCommand(const std::string& command) {
    return system(command.c_str());
}
