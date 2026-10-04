#!/bin/bash

set -e

echo "Uninstalling Device Health Monitor..."

# Stop and disable the service
sudo systemctl stop device_health_monitor.service 2>/dev/null || true
sudo systemctl disable device_health_monitor.service 2>/dev/null || true

# Remove systemd service
sudo rm -f /etc/systemd/system/device_health_monitor.service

# Reload systemd
sudo systemctl daemon-reload

# Remove executable
sudo rm -f /usr/local/bin/device_health_monitor

# Remove config
sudo rm -rf /etc/device_health_monitor

echo "Uninstallation complete!"
