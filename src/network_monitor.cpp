#include "network_monitor.h"
#include <fstream>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

NetworkMonitor::NetworkMonitor() {}

NetworkMonitor::~NetworkMonitor() {}

bool NetworkMonitor::isNetworkUp(const std::string& interface) {
    if (!checkInterfaceExists(interface)) {
        return false;
    }

    std::ifstream file("/sys/class/net/" + interface + "/operstate");
    if (!file.is_open()) {
        return false;
    }

    std::string state;
    std::getline(file, state);
    return state == "up";
}

bool NetworkMonitor::hasInternetConnection() {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        return false;
    }

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(53);
    
    // Use Google DNS as test
    inet_pton(AF_INET, "8.8.8.8", &server.sin_addr);

    int connectResult = connect(sock, (struct sockaddr*)&server, sizeof(server));
    close(sock);

    return connectResult == 0;
}

long NetworkMonitor::getBytesTransmitted(const std::string& interface) {
    return std::stol(readNetworkStat(interface, "tx_bytes"));
}

long NetworkMonitor::getBytesReceived(const std::string& interface) {
    return std::stol(readNetworkStat(interface, "rx_bytes"));
}

std::string NetworkMonitor::getNetworkStatus(const std::string& interface) {
    std::ostringstream status;
    status << "Interface: " << interface << "\n";
    status << "State: " << (isNetworkUp(interface) ? "UP" : "DOWN") << "\n";
    status << "Has Internet: " << (hasInternetConnection() ? "YES" : "NO") << "\n";
    status << "Bytes Transmitted: " << getBytesTransmitted(interface) << "\n";
    status << "Bytes Received: " << getBytesReceived(interface);
    return status.str();
}

bool NetworkMonitor::checkInterfaceExists(const std::string& interface) {
    std::ifstream file("/sys/class/net/" + interface + "/address");
    return file.is_open();
}

std::string NetworkMonitor::readNetworkStat(const std::string& interface, const std::string& stat) {
    std::ifstream file("/sys/class/net/" + interface + "/statistics/" + stat);
    if (!file.is_open()) {
        return "0";
    }
    std::string value;
    std::getline(file, value);
    return value;
}
