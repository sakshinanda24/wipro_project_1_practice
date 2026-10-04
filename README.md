# Embedded Linux Device Health Monitor & Auto-Recovery Agent

A C/C++ background service that continuously monitors Linux system health metrics and automatically recovers from failures without manual intervention.

---

## Table of Contents

- [Features](#features)
- [Use Cases](#use-cases)
- [Prerequisites](#prerequisites)
- [Project Structure](#project-structure)
- [Build](#build)
- [Installation](#installation)
- [Service Management](#service-management)
- [Configuration](#configuration)
- [Viewing Logs](#viewing-logs)
- [Failure Simulation & Testing](#failure-simulation--testing)
- [Manual Monitoring Commands](#manual-monitoring-commands)
- [Troubleshooting](#troubleshooting)
- [Uninstallation](#uninstallation)

---

## Features

| Monitor | What It Tracks | Auto-Recovery |
|---|---|---|
| CPU | Usage % via `/proc/stat` | Kills runaway processes above threshold |
| Memory | RAM usage % via `/proc/meminfo` | Frees memory by killing high-usage processes |
| Disk | Disk usage % via `statvfs()` | Alerts on critical usage |
| Temperature | CPU temp via `/sys/class/thermal/` | Alerts on warning/critical thresholds |
| Network | Interface state & internet connectivity | Alerts on interface down |
| Service | Critical service running state | Auto-restarts failed services |

---

## Use Cases

- **Embedded Linux devices** (Raspberry Pi, industrial boards) that need unattended operation
- **Headless servers** where manual monitoring is not feasible
- **IoT gateways** requiring high availability of critical services like `sshd` or `mosquitto`
- **Edge computing nodes** where thermal and resource management is critical
- **CI/CD environments** needing automated health checks and recovery

---

## Prerequisites

- Linux kernel 4.0+
- GCC 9+ or Clang
- CMake 3.10+
- systemd
- `stress` (optional, for failure simulation): `sudo apt install stress`

---

## Project Structure

```
.
├── config/                  # Default configuration
│   └── health_monitor.json
├── docs/                    # Documentation
│   ├── USER_MANUAL.md
│   ├── architecture.md
│   └── test-report.md
├── include/                 # Header files
├── scripts/
│   ├── install.sh           # Full install script
│   ├── uninstall.sh         # Full uninstall script
│   └── simulate_failure.sh  # Test failure scenarios
├── src/                     # Source files
├── systemd/
│   └── device_health_monitor.service
├── tests/                   # Unit tests
└── CMakeLists.txt
```

---

## Build

```bash
mkdir build && cd build
cmake ..
make
```

The binary is placed at `build/device_health_monitor`.

---

## Installation

### Option 1 — Automated (Recommended)

Builds, installs, and starts the systemd service in one step:

```bash
sudo ./scripts/install.sh
```

This will:
- Build the project
- Install binary to `/usr/local/bin/device_health_monitor`
- Copy config to `/etc/device_health_monitor/health_monitor.json`
- Register and start the systemd service

### Option 2 — Manual

```bash
mkdir build && cd build
cmake ..
make
sudo make install
sudo cp systemd/device_health_monitor.service /etc/systemd/system/
sudo systemctl daemon-reload
sudo systemctl enable device_health_monitor
sudo systemctl start device_health_monitor
```

---

## Service Management

```bash
# Start
sudo systemctl start device_health_monitor

# Stop
sudo systemctl stop device_health_monitor

# Restart
sudo systemctl restart device_health_monitor

# Check status
sudo systemctl status device_health_monitor

# Enable auto-start on boot
sudo systemctl enable device_health_monitor

# Disable auto-start on boot
sudo systemctl disable device_health_monitor
```

### Run Manually (Foreground / Debug Mode)

```bash
# Stop the service first to avoid conflicts
sudo systemctl stop device_health_monitor

# Run in foreground
sudo /usr/local/bin/device_health_monitor

# Run with debug logging
sudo LOG_LEVEL=DEBUG /usr/local/bin/device_health_monitor
```

---

## Configuration

Config file location after install: `/etc/device_health_monitor/health_monitor.json`

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
    "network": {
        "interface": "eth0",
        "check_interval_seconds": 30
    },
    "service_monitor": {
        "critical_service": "sshd",
        "restart_on_failure": true
    },
    "recovery": {
        "kill_process_threshold_percent": 95,
        "max_restart_attempts": 3,
        "restart_delay_seconds": 5
    },
    "logging": {
        "level": "INFO",
        "file": "/var/log/device_health_monitor.log",
        "max_size_mb": 10,
        "max_files": 5
    }
}
```

### Key Parameters

| Parameter | Default | Description |
|---|---|---|
| `monitoring_interval_seconds` | 10 | How often health checks run (seconds) |
| `cpu_usage_percent` | 80 | CPU warning threshold (%) |
| `memory_usage_percent` | 85 | RAM warning threshold (%) |
| `disk_usage_percent` | 90 | Disk warning threshold (%) |
| `temperature_critical_celsius` | 85 | Critical temp — triggers recovery |
| `temperature_warning_celsius` | 75 | Warning temp — logs alert |
| `interface` | eth0 | Network interface to monitor |
| `critical_service` | sshd | Service to monitor and auto-restart |
| `restart_on_failure` | true | Enable auto-restart of failed service |
| `max_restart_attempts` | 3 | Max restart attempts before giving up |
| `kill_process_threshold_percent` | 95 | CPU % at which runaway processes are killed |
| `log level` | INFO | `DEBUG` / `INFO` / `WARNING` / `ERROR` / `CRITICAL` |

### Applying Config Changes

```bash
sudo nano /etc/device_health_monitor/health_monitor.json
sudo systemctl restart device_health_monitor
```

---

## Viewing Logs

### Via journalctl

```bash
# Follow live logs
sudo journalctl -u device_health_monitor -f

# Last 100 lines
sudo journalctl -u device_health_monitor -n 100

# Logs from today
sudo journalctl -u device_health_monitor --since "today"

# Logs from a specific time
sudo journalctl -u device_health_monitor --since "2024-01-01 10:00:00"
```

### Via log file

```bash
# Follow live
sudo tail -f /var/log/device_health_monitor.log

# Search by severity
sudo grep "WARNING"  /var/log/device_health_monitor.log
sudo grep "ERROR"    /var/log/device_health_monitor.log
sudo grep "CRITICAL" /var/log/device_health_monitor.log
```

### Log Format

```
[YYYY-MM-DD HH:MM:SS.mmm] [LEVEL] Message
```

Example output:
```
[2024-01-15 10:30:10.457] [DEBUG]    CPU Usage: 25.5%
[2024-01-15 10:30:10.458] [DEBUG]    Memory Usage: 65.2%
[2024-01-15 10:30:10.461] [INFO]     Network interface eth0 is UP
[2024-01-15 10:30:10.462] [INFO]     Critical service sshd is running
[2024-01-15 10:30:20.789] [WARNING]  CPU usage exceeded threshold: 85% > 80%
[2024-01-15 10:30:30.012] [CRITICAL] Critical temperature reached: 90°C >= 85°C
```

---

## Failure Simulation & Testing

Use the simulation script to test recovery behavior:

```bash
# Simulate high CPU load (30 seconds)
sudo ./scripts/simulate_failure.sh cpu

# Simulate high memory usage (30 seconds)
sudo ./scripts/simulate_failure.sh memory

# Simulate high disk usage
sudo ./scripts/simulate_failure.sh disk

# Simulate high temperature
sudo ./scripts/simulate_failure.sh temperature

# Simulate network interface down
sudo ./scripts/simulate_failure.sh network

# Simulate critical service (sshd) failure
sudo ./scripts/simulate_failure.sh service

# Reset all simulated conditions
sudo ./scripts/simulate_failure.sh reset
```

### Unit Tests

```bash
g++ -o test_cpu            tests/test_cpu.cpp            src/cpu_monitor.cpp     -lcppunit
g++ -o test_memory         tests/test_memory.cpp         src/memory_monitor.cpp  -lcppunit
g++ -o test_config         tests/test_config.cpp         src/config_manager.cpp  -lcppunit
g++ -o test_service_monitor tests/test_service_monitor.cpp src/service_monitor.cpp -lcppunit

./test_cpu
./test_memory
./test_config
./test_service_monitor
```

---

## Manual Monitoring Commands

```bash
# CPU usage
top -bn1 | head -5

# Memory usage
free -h

# Disk usage
df -h

# Temperature (divide by 1000 for °C)
cat /sys/class/thermal/thermal_zone0/temp

# Network interface state
cat /sys/class/net/eth0/operstate

# Service status
systemctl status sshd
```

---

## Troubleshooting

**Service won't start**
```bash
sudo systemctl status device_health_monitor
sudo journalctl -u device_health_monitor -n 50 --no-pager
ls -la /usr/local/bin/device_health_monitor
```

**Permission denied**
```bash
# Always run install/uninstall with sudo
sudo ./scripts/install.sh
```

**False alarms / too noisy**
```bash
# Raise thresholds in config, then restart
sudo nano /etc/device_health_monitor/health_monitor.json
sudo systemctl restart device_health_monitor
```

**Log file too large**
```bash
# Truncate without stopping service
sudo truncate -s 0 /var/log/device_health_monitor.log
```

**Service not auto-restarting**
```bash
# Verify restart_on_failure is true in config
grep restart_on_failure /etc/device_health_monitor/health_monitor.json

# Check restart attempt logs
sudo journalctl -u device_health_monitor | grep "restart"
```

---

## Uninstallation

```bash
sudo ./scripts/uninstall.sh
```

This will:
- Stop and disable the systemd service
- Remove `/usr/local/bin/device_health_monitor`
- Remove `/etc/device_health_monitor/`
- Remove `/etc/systemd/system/device_health_monitor.service`

> Logs at `/var/log/device_health_monitor.log` are preserved. Remove manually if needed:
> ```bash
> sudo rm /var/log/device_health_monitor.log
> ```
