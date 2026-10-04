#!/bin/bash

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"

echo "Installing Device Health Monitor..."

# Create config directory
sudo mkdir -p /etc/device_health_monitor

# Copy configuration
sudo cp "$PROJECT_DIR/config/health_monitor.json" /etc/device_health_monitor/

# Build the project
echo "Building project..."
mkdir -p "$PROJECT_DIR/build"
cd "$PROJECT_DIR/build"
cmake ..
make

# Install the executable
sudo make install

# Copy systemd service
sudo cp "$PROJECT_DIR/systemd/device_health_monitor.service" /etc/systemd/system/

# Reload systemd
sudo systemctl daemon-reload

# Enable and start the service
sudo systemctl enable device_health_monitor.service
sudo systemctl start device_health_monitor.service

echo "Installation complete!"
echo "Service status: sudo systemctl status device_health_monitor"
echo "View logs: sudo journalctl -u device_health_monitor -f"
