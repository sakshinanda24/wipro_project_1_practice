# Build and Run Instructions

## Prerequisites

- CMake 3.10 or higher
- GCC 9 or higher
- Linux kernel 4.0 or higher
- systemd (for service management)

## Building

```bash
mkdir build && cd build
cmake ..
make
```

## Installing

```bash
sudo make install
```

This will:
- Install the executable to `/usr/local/bin/device_health_monitor`
- Install config to `/etc/device_health_monitor/health_monitor.json`
- Install systemd service to `/etc/systemd/system/device_health_monitor.service`

## Running

### As a Service (Recommended)

```bash
# Start the service
sudo systemctl start device_health_monitor

# Enable on boot
sudo systemctl enable device_health_monitor

# Check status
sudo systemctl status device_health_monitor

# View logs
sudo journalctl -u device_health_monitor -f
```

### Manual Run

```bash
# Run in foreground
sudo /usr/local/bin/device_health_monitor

# Run with debug logging
LOG_LEVEL=DEBUG /usr/local/bin/device_health_monitor
```

## Configuration

Edit `/etc/device_health_monitor/health_monitor.json` to customize:

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

## Testing

### Unit Tests

```bash
# Build tests
g++ -o test_cpu tests/test_cpu.cpp src/cpu_monitor.cpp -lcppunit
g++ -o test_memory tests/test_memory.cpp src/memory_monitor.cpp -lcppunit
g++ -o test_config tests/test_config.cpp src/config_manager.cpp -lcppunit
g++ -o test_service_monitor tests/test_service_monitor.cpp src/service_monitor.cpp -lcppunit

# Run tests
./test_cpu
./test_memory
./test_config
./test_service_monitor
```

### Integration Tests

Use the simulation script to test various failure scenarios:

```bash
# Simulate high CPU
sudo ./scripts/simulate_failure.sh cpu

# Simulate high memory
sudo ./scripts/simulate_failure.sh memory

# Simulate service failure
sudo ./scripts/simulate_failure.sh service

# Reset all simulations
sudo ./scripts/simulate_failure.sh reset
```

## Uninstalling

```bash
sudo ./scripts/uninstall.sh
```

## Troubleshooting

### Service won't start
```bash
# Check logs
sudo journalctl -u device_health_monitor -n 50

# Check permissions
ls -la /etc/device_health_monitor/
```

### High CPU usage
```bash
# Check what's running
ps aux --sort=-%cpu | head -10

# Adjust monitoring interval in config
```

### Permission denied
```bash
# Ensure running as root or with sudo
sudo ./scripts/install.sh
```
