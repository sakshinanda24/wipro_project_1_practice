#!/bin/bash

# Simple build script for quick testing
# This script builds the project without requiring CMake installation

set -e

echo "Building Device Health Monitor..."

# Create build directory
mkdir -p build
cd build

# Compile source files
g++ -std=c++17 -I../include -o device_health_monitor \
    ../src/main.cpp \
    ../src/cpu_monitor.cpp \
    ../src/memory_monitor.cpp \
    ../src/disk_monitor.cpp \
    ../src/temperature_monitor.cpp \
    ../src/network_monitor.cpp \
    ../src/service_monitor.cpp \
    ../src/logger.cpp \
    ../src/config_manager.cpp \
    -lpthread

echo "Build complete!"
echo "Executable: build/device_health_monitor"
echo ""
echo "To install:"
echo "  sudo cp build/device_health_monitor /usr/local/bin/"
echo "  sudo mkdir -p /etc/device_health_monitor"
echo "  sudo cp ../config/health_monitor.json /etc/device_health_monitor/"
echo "  sudo cp ../systemd/device_health_monitor.service /etc/systemd/system/"
echo "  sudo systemctl daemon-reload"
echo "  sudo systemctl enable device_health_monitor"
echo "  sudo systemctl start device_health_monitor"
