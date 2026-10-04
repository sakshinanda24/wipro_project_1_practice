#include <iostream>
#include <thread>
#include <chrono>
#include <csignal>
#include <unistd.h>
#include <sys/stat.h>
#include <fstream>

#include "cpu_monitor.h"
#include "memory_monitor.h"
#include "disk_monitor.h"
#include "temperature_monitor.h"
#include "network_monitor.h"
#include "service_monitor.h"
#include "logger.h"
#include "config_manager.h"

volatile std::sig_atomic_t runLoop = 1;

void signalHandler(int signum) {
    (void)signum;
    runLoop = 0;
}

void checkCPU(MonitorConfig& config) {
    CPUMonitor cpuMon;
    double usage = cpuMon.getCPUUsage();
    
    Logger& logger = Logger::getInstance();
    logger.log(LogLevel::DEBUG, "CPU Usage: " + std::to_string(usage) + "%");
    
    if (cpuMon.isCPUOverloaded(config.cpuUsagePercent)) {
        logger.log(LogLevel::WARNING, "CPU usage exceeded threshold: " + std::to_string(usage) + "% > " + std::to_string(config.cpuUsagePercent) + "%");
        
        // Kill high CPU processes if needed
        if (usage > config.killProcessThresholdPercent) {
            logger.log(LogLevel::CRITICAL, "CPU usage critical, killing processes...");
            // In production, you would implement process killing logic here
        }
    }
}

void checkMemory(MonitorConfig& config) {
    MemoryMonitor memMon;
    double usage = memMon.getMemoryUsagePercent();
    
    Logger& logger = Logger::getInstance();
    logger.log(LogLevel::DEBUG, "Memory Usage: " + std::to_string(usage) + "%");
    
    if (memMon.isMemoryOverloaded(config.memoryUsagePercent)) {
        logger.log(LogLevel::WARNING, "Memory usage exceeded threshold: " + std::to_string(usage) + "% > " + std::to_string(config.memoryUsagePercent) + "%");
    }
}

void checkDisk(MonitorConfig& config) {
    DiskMonitor diskMon;
    double usage = diskMon.getDiskUsagePercent("/");
    
    Logger& logger = Logger::getInstance();
    logger.log(LogLevel::DEBUG, "Disk Usage: " + std::to_string(usage) + "%");
    
    if (diskMon.isDiskOverloaded(config.diskUsagePercent, "/")) {
        logger.log(LogLevel::WARNING, "Disk usage exceeded threshold: " + std::to_string(usage) + "% > " + std::to_string(config.diskUsagePercent) + "%");
    }
}

void checkTemperature(MonitorConfig& config) {
    TemperatureMonitor tempMon;
    double temp = tempMon.getSystemTemperature();
    
    Logger& logger = Logger::getInstance();
    logger.log(LogLevel::DEBUG, "System Temperature: " + std::to_string(temp) + "°C");
    
    if (tempMon.isTemperatureCritical(config.temperatureCriticalCelsius)) {
        logger.log(LogLevel::CRITICAL, "Critical temperature reached: " + std::to_string(temp) + "°C >= " + std::to_string(config.temperatureCriticalCelsius) + "°C");
    } else if (tempMon.isTemperatureWarning(config.temperatureWarningCelsius)) {
        logger.log(LogLevel::WARNING, "Temperature warning: " + std::to_string(temp) + "°C >= " + std::to_string(config.temperatureWarningCelsius) + "°C");
    }
}

void checkNetwork(MonitorConfig& config) {
    NetworkMonitor netMon;
    
    Logger& logger = Logger::getInstance();
    
    if (!netMon.isNetworkUp(config.networkInterface)) {
        logger.log(LogLevel::ERROR, "Network interface " + config.networkInterface + " is down");
    }
    
    if (!netMon.hasInternetConnection()) {
        logger.log(LogLevel::WARNING, "No internet connection");
    }
}

void checkService(MonitorConfig& config) {
    ServiceMonitor svcMon;
    
    Logger& logger = Logger::getInstance();
    
    if (!svcMon.isServiceRunning(config.criticalService)) {
        logger.log(LogLevel::CRITICAL, "Critical service " + config.criticalService + " is not running");
        
        if (config.restartOnFailure) {
            logger.log(LogLevel::INFO, "Attempting to restart " + config.criticalService);
            
            for (int attempt = 1; attempt <= config.maxRestartAttempts; attempt++) {
                logger.log(LogLevel::INFO, "Restart attempt " + std::to_string(attempt) + "/" + std::to_string(config.maxRestartAttempts));
                
                if (svcMon.startService(config.criticalService)) {
                    logger.log(LogLevel::INFO, "Successfully restarted " + config.criticalService);
                    break;
                }
                
                logger.log(LogLevel::WARNING, "Failed to restart " + config.criticalService + ", retrying in " + std::to_string(config.restartDelaySeconds) + "s");
                std::this_thread::sleep_for(std::chrono::seconds(config.restartDelaySeconds));
            }
        }
    }
}

int main() {
    // Setup signal handlers
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);

    // Initialize logger
    Logger& logger = Logger::getInstance();
    logger.log(LogLevel::INFO, "Device Health Monitor starting...");

    // Load configuration
    ConfigManager& configMgr = ConfigManager::getInstance();
    std::string configPath = "/etc/device_health_monitor/health_monitor.json";
    
    if (!configMgr.loadConfig(configPath)) {
        logger.log(LogLevel::WARNING, "Using default configuration");
    }
    
    MonitorConfig config = configMgr.getConfig();
    logger.log(LogLevel::INFO, "Configuration loaded successfully");

    // Create log directory if it doesn't exist
    mkdir("/var/log", 0755);

    logger.log(LogLevel::INFO, "Monitoring interval: " + std::to_string(config.monitoringIntervalSeconds) + "s");
    logger.log(LogLevel::INFO, "Critical service: " + config.criticalService);

    // Main monitoring loop
    int networkCheckCounter = 0;
    
    while (runLoop) {
        logger.log(LogLevel::DEBUG, "=== Health Check Cycle ===");
        
        checkCPU(config);
        checkMemory(config);
        checkDisk(config);
        checkTemperature(config);
        
        // Check network less frequently
        networkCheckCounter++;
        if (networkCheckCounter >= config.networkCheckIntervalSeconds / config.monitoringIntervalSeconds) {
            checkNetwork(config);
            networkCheckCounter = 0;
        }
        
        checkService(config);
        
        logger.log(LogLevel::DEBUG, "=== End Health Check Cycle ===");
        
        // Wait for next interval
        for (int i = 0; i < config.monitoringIntervalSeconds && runLoop; i++) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

    logger.log(LogLevel::INFO, "Device Health Monitor shutting down...");
    return 0;
}
