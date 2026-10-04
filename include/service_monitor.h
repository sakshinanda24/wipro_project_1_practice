#ifndef SERVICE_MONITOR_H
#define SERVICE_MONITOR_H

#include <string>

class ServiceMonitor {
public:
    ServiceMonitor();
    ~ServiceMonitor();

    bool isServiceRunning(const std::string& serviceName);
    bool startService(const std::string& serviceName);
    bool stopService(const std::string& serviceName);
    bool restartService(const std::string& serviceName);
    int getServicePID(const std::string& serviceName);
    std::string getServiceStatus(const std::string& serviceName);

private:
    int executeCommand(const std::string& command);
};

#endif // SERVICE_MONITOR_H
