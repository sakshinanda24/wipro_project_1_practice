# Architecture Documentation

## Overview

The Device Health Monitor is a C++ application that runs as a background service on Linux systems. It continuously monitors system health metrics and automatically recovers from failures.

## System Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                     Main Application                         │
│  ┌───────────────────────────────────────────────────────┐  │
│  │              Monitoring Loop (Main Thread)            │  │
│  │  ┌─────────┐ ┌─────────┐ ┌─────────┐ ┌─────────────┐ │  │
│  │  │  CPU    │ │ Memory  │ │  Disk   │ │ Temperature │ │  │
│  │  │ Monitor │ │ Monitor │ │ Monitor │ │  Monitor    │ │  │
│  │  └─────────┘ └─────────┘ └─────────┘ └─────────────┘ │  │
│  │  ┌─────────┐ ┌─────────┐ ┌─────────┐                 │  │
│  │  │ Network │ │ Service │ │  Logger │                 │  │
│  │  │ Monitor │ │ Monitor │ │         │                 │  │
│  │  └─────────┘ └─────────┘ └─────────┘                 │  │
│  └───────────────────────────────────────────────────────┘  │
│                        │                                     │
│  ┌─────────────────────┴─────────────────────────────────┐  │
│  │              Configuration Manager                     │  │
│  │              (JSON-based config)                       │  │
│  └────────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────┘
```

## Components

### 1. Main Application (main.cpp)
- Entry point for the application
- Manages the monitoring loop
- Coordinates all monitors
- Handles signals (SIGINT, SIGTERM)
- Loads configuration

### 2. Monitors

#### CPU Monitor (cpu_monitor.cpp)
- Reads CPU statistics from `/proc/stat`
- Calculates CPU usage percentage
- Detects overloaded conditions

#### Memory Monitor (memory_monitor.cpp)
- Reads memory info from `/proc/meminfo`
- Calculates memory usage percentage
- Monitors available memory

#### Disk Monitor (disk_monitor.cpp)
- Uses `statvfs()` to get disk statistics
- Calculates disk usage percentage
- Monitors free space

#### Temperature Monitor (temperature_monitor.cpp)
- Reads from `/sys/class/thermal/`
- Monitors CPU/system temperature
- Detects critical/warning temperatures

#### Network Monitor (network_monitor.cpp)
- Checks interface status from `/sys/class/net/`
- Tests internet connectivity
- Monitors network statistics

#### Service Monitor (service_monitor.cpp)
- Uses `systemctl` to manage services
- Checks if services are running
- Restarts failed services

### 3. Logger (logger.cpp)
- Thread-safe logging
- Log rotation
- Multiple log levels (DEBUG, INFO, WARNING, ERROR, CRITICAL)
- Console and file output

### 4. Config Manager (config_manager.cpp)
- Loads configuration from JSON file
- Parses configuration values
- Provides default values if config is missing

## Data Flow

1. **Initialization**
   - Load configuration from `/etc/device_health_monitor/health_monitor.json`
   - Initialize all monitors
   - Set up signal handlers

2. **Monitoring Loop**
   - Every N seconds (configurable):
     - Check CPU usage
     - Check memory usage
     - Check disk usage
     - Check temperature
     - Check network status (every M seconds)
     - Check critical service status
   - If threshold exceeded → Log warning
   - If critical → Attempt recovery

3. **Recovery Actions**
   - Log the issue
   - Attempt to restart failed service
   - Kill runaway processes if needed
   - Log recovery attempts

## Configuration

Configuration is stored in JSON format at `/etc/device_health_monitor/health_monitor.json`:

```json
{
    "monitoring_interval_seconds": 10,
    "thresholds": {
        "cpu_usage_percent": 80,
        "memory_usage_percent": 85,
        "disk_usage_percent": 90,
        "temperature_critical_celsius": 85,
        "temperature_warning_celsius": 75
    },
    "service_monitor": {
        "critical_service": "sshd",
        "restart_on_failure": true
    }
}
```

## File Locations

- Executable: `/usr/local/bin/device_health_monitor`
- Config: `/etc/device_health_monitor/health_monitor.json`
- Logs: `/var/log/device_health_monitor.log`
- Service: `/etc/systemd/system/device_health_monitor.service`

## Build System

Uses CMake for building:

```bash
mkdir build && cd build
cmake ..
make
sudo make install
```

## Testing

Unit tests are provided for each component:
- `test_cpu.cpp` - CPU monitor tests
- `test_memory.cpp` - Memory monitor tests
- `test_config.cpp` - Configuration manager tests
- `test_service_monitor.cpp` - Service monitor tests

Run tests with:
```bash
g++ -o test_cpu tests/test_cpu.cpp src/cpu_monitor.cpp -lcppunit
./test_cpu
```
