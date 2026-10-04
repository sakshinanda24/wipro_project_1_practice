# Device Health Monitor - User Manual

## Overview

The Device Health Monitor is a background service that continuously monitors your Linux system's health metrics including CPU usage, memory, disk space, temperature, network status, and critical services. It automatically takes recovery actions when issues are detected.

## Quick Start

### Installation

```bash
sudo ./scripts/install.sh
```

### Uninstallation

```bash
sudo ./scripts/uninstall.sh
```

## Starting and Stopping the Service

### Start the Monitor

```bash
sudo systemctl start device_health_monitor
```

### Stop the Monitor

```bash
sudo systemctl stop device_health_monitor
```

### Restart the Monitor

```bash
sudo systemctl restart device_health_monitor
```

### Enable on Boot

```bash
sudo systemctl enable device_health_monitor
```

### Disable on Boot

```bash
sudo systemctl disable device_health_monitor
```

### Check Service Status

```bash
sudo systemctl status device_health_monitor
```

## Reading Monitor Data

### View Real-time Logs

```bash
# View last 100 lines
sudo journalctl -u device_health_monitor -n 100

# Follow logs in real-time
sudo journalctl -u device_health_monitor -f

# View logs from today
sudo journalctl -u device_health_monitor --since "today"

# View logs from a specific time
sudo journalctl -u device_health_monitor --since "2024-01-01 10:00:00"
```

### View Log File Directly

```bash
# View the log file
sudo cat /var/log/device_health_monitor.log

# Follow the log file
sudo tail -f /var/log/device_health_monitor.log

# View last 50 lines
sudo tail -n 50 /var/log/device_health_monitor.log

# Search for specific terms
sudo grep "WARNING" /var/log/device_health_monitor.log
sudo grep "ERROR" /var/log/device_health_monitor.log
sudo grep "CRITICAL" /var/log/device_health_monitor.log
```

## Understanding Log Output

Log entries follow this format:
```
[YYYY-MM-DD HH:MM:SS.mmm] [LEVEL] Message
```

### Log Levels

| Level | Description | Example |
|-------|-------------|---------|
| DEBUG | Detailed diagnostic information | "CPU Usage: 25.5%" |
| INFO | Normal operational messages | "Device Health Monitor starting..." |
| WARNING | Threshold exceeded | "CPU usage exceeded threshold: 85% > 80%" |
| ERROR | Network or service issues | "Network interface eth0 is down" |
| CRITICAL | Recovery actions needed | "Critical temperature reached: 90°C" |

### Sample Log Entries

```
[2024-01-15 10:30:00.123] [INFO] Device Health Monitor starting...
[2024-01-15 10:30:00.125] [INFO] Configuration loaded successfully
[2024-01-15 10:30:00.126] [INFO] Monitoring interval: 10s
[2024-01-15 10:30:00.127] [INFO] Critical service: sshd
[2024-01-15 10:30:10.456] [DEBUG] === Health Check Cycle ===
[2024-01-15 10:30:10.457] [DEBUG] CPU Usage: 25.5%
[2024-01-15 10:30:10.458] [DEBUG] Memory Usage: 65.2%
[2024-01-15 10:30:10.459] [DEBUG] Disk Usage: 45.8%
[2024-01-15 10:30:10.460] [DEBUG] System Temperature: 55.0°C
[2024-01-15 10:30:10.461] [INFO] Network interface eth0 is UP
[2024-01-15 10:30:10.462] [INFO] Critical service sshd is running
[2024-01-15 10:30:10.463] [DEBUG] === End Health Check Cycle ===
[2024-01-15 10:30:20.789] [WARNING] CPU usage exceeded threshold: 85% > 80%
[2024-01-15 10:30:30.012] [CRITICAL] Critical temperature reached: 90°C >= 85°C
```

## Configuration

### Configuration File Location

```
/etc/device_health_monitor/health_monitor.json
```

### Current Configuration

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

### Modifying Configuration

1. Edit the configuration file:
   ```bash
   sudo nano /etc/device_health_monitor/health_monitor.json
   ```

2. Restart the service:
   ```bash
   sudo systemctl restart device_health_monitor
   ```

### Configuration Parameters

| Parameter | Description | Default | Range |
|-----------|-------------|---------|-------|
| monitoring_interval_seconds | How often to check system health | 10 | 1-300 |
| cpu_usage_percent | CPU usage warning threshold | 80 | 0-100 |
| memory_usage_percent | Memory usage warning threshold | 85 | 0-100 |
| disk_usage_percent | Disk usage warning threshold | 90 | 0-100 |
| temperature_critical_celsius | Critical temperature threshold | 85 | 0-150 |
| temperature_warning_celsius | Warning temperature threshold | 75 | 0-150 |
| interface | Network interface to monitor | eth0 | Any interface |
| critical_service | Service to monitor and restart | sshd | Any service |
| restart_on_failure | Auto-restart failed services | true | true/false |
| max_restart_attempts | Max restart attempts before giving up | 3 | 1-10 |
| restart_delay_seconds | Delay between restart attempts | 5 | 1-60 |
| log_level | Logging verbosity | INFO | DEBUG/INFO/WARNING/ERROR/CRITICAL |

## Monitoring Metrics

### CPU Usage

- **Source**: `/proc/stat`
- **Unit**: Percentage
- **Example**: 25.5% means CPU is busy 25.5% of the time

### Memory Usage

- **Source**: `/proc/meminfo`
- **Unit**: Percentage and MB
- **Example**: 65.2% used, 2048 MB used

### Disk Usage

- **Source**: `statvfs()` system call
- **Unit**: Percentage and GB
- **Example**: 45.8% used, 150 GB free

### Temperature

- **Source**: `/sys/class/thermal/`
- **Unit**: Celsius
- **Example**: 55.0°C

### Network Status

- **Source**: `/sys/class/net/`
- **Checks**: Interface state and internet connectivity
- **Example**: eth0 UP, Internet: YES

### Service Status

- **Source**: `systemctl` commands
- **Checks**: Service running state
- **Example**: sshd is running (PID: 1234)

## Manual Monitoring

### Check CPU Usage

```bash
# Using top
top -bn1 | head -20

# Using htop (if installed)
htop

# Using /proc/stat
cat /proc/stat | head -1
```

### Check Memory Usage

```bash
# Using free
free -h

# Using /proc/meminfo
cat /proc/meminfo | head -20
```

### Check Disk Usage

```bash
# Using df
df -h

# Check specific path
df -h /
```

### Check Temperature

```bash
# Using sensors (if lm-sensors installed)
sensors

# Using sysfs
cat /sys/class/thermal/thermal_zone0/temp
# Divide by 1000 to get Celsius
```

### Check Network

```bash
# Check interface status
cat /sys/class/net/eth0/operstate

# Check connectivity
ping -c 1 8.8.8.8
```

### Check Services

```bash
# Check service status
systemctl status sshd

# Check if running
pgrep -x sshd
```

## Troubleshooting

### Service Won't Start

```bash
# Check service status
sudo systemctl status device_health_monitor

# View error logs
sudo journalctl -u device_health_monitor -n 50 --no-pager

# Check if executable exists
ls -la /usr/local/bin/device_health_monitor

# Check config file
cat /etc/device_health_monitor/health_monitor.json
```

### High CPU Usage

```bash
# Check what's using CPU
ps aux --sort=-%cpu | head -10

# Increase monitoring interval in config
sudo nano /etc/device_health_monitor/health_monitor.json
# Change monitoring_interval_seconds to 30 or higher
sudo systemctl restart device_health_monitor
```

### Log File Too Large

```bash
# View log size
ls -lh /var/log/device_health_monitor.log

# Rotate logs manually
sudo systemctl restart device_health_monitor

# Or delete old logs (keeps current)
sudo truncate -s 0 /var/log/device_health_monitor.log
```

### False Alarms

```bash
# Adjust thresholds in config
sudo nano /etc/device_health_monitor/health_monitor.json

# Example: Increase CPU threshold from 80% to 90%
# "cpu_usage_percent": 90,

# Restart service
sudo systemctl restart device_health_monitor
```

### Service Not Restarting

```bash
# Check if service exists
systemctl list-units --type=service | grep sshd

# Check if service monitor is enabled
grep restart_on_failure /etc/device_health_monitor/health_monitor.json

# Manually restart service
sudo systemctl restart sshd

# Check logs for restart attempts
sudo journalctl -u device_health_monitor | grep "sshd"
```

## Advanced Usage

### Run in Foreground for Testing

```bash
# Stop the service first
sudo systemctl stop device_health_monitor

# Run manually with debug logging
sudo /usr/local/bin/device_health_monitor

# Or with specific log level
sudo LOG_LEVEL=DEBUG /usr/local/bin/device_health_monitor
```

### Custom Monitoring Script

```bash
# Create a script to check health
cat > /usr/local/bin/check_health.sh << 'EOF'
#!/bin/bash
# Check CPU
cpu=$(cat /proc/stat | head -1 | awk '{usage=($1+$2+$3)*100/($1+$2+$3+$4)} END {print int(usage)}')
echo "CPU: ${cpu}%"

# Check memory
mem=$(free | grep Mem | awk '{print int($3/$2 * 100)}')
echo "Memory: ${mem}%"

# Check disk
disk=$(df / | tail -1 | awk '{print $5}' | tr -d '%')
echo "Disk: ${disk}%"

# Check temperature
temp=$(cat /sys/class/thermal/thermal_zone0/temp 2>/dev/null)
if [ -n "$temp" ]; then
    echo "Temperature: $(echo "scale=1; $temp/1000" | bc)°C"
fi
EOF

chmod +x /usr/local/bin/check_health.sh
```

### Integration with Monitoring Tools

#### Prometheus Exporter

Create a simple HTTP endpoint for Prometheus:

```bash
# Install nginx or use Python
python3 -m http.server 8080
```

#### Grafana Dashboard

1. Install Telegraf or collectd
2. Configure to collect system metrics
3. Import pre-built dashboard in Grafana

## Support

For issues or questions:
1. Check the logs: `sudo journalctl -u device_health_monitor -f`
2. Review the configuration: `cat /etc/device_health_monitor/health_monitor.json`
3. Test with simulation script: `sudo ./scripts/simulate_failure.sh cpu`

## Changelog

### Version 1.0.0
- Initial release
- CPU, memory, disk, temperature, network, and service monitoring
- Automatic recovery actions
- JSON configuration
- Systemd service integration
- Comprehensive logging
