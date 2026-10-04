#ifndef NETWORK_MONITOR_H
#define NETWORK_MONITOR_H

#include <string>

class NetworkMonitor {
public:
    NetworkMonitor();
    ~NetworkMonitor();

    bool isNetworkUp(const std::string& interface = "eth0");
    bool hasInternetConnection();
    long getBytesTransmitted(const std::string& interface = "eth0");
    long getBytesReceived(const std::string& interface = "eth0");
    std::string getNetworkStatus(const std::string& interface = "eth0");

private:
    bool checkInterfaceExists(const std::string& interface);
    std::string readNetworkStat(const std::string& interface, const std::string& stat);
};

#endif // NETWORK_MONITOR_H
