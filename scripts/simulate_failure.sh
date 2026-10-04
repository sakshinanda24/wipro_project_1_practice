#!/bin/bash

# Script to simulate various failure conditions for testing

echo "=== Device Health Monitor Test Simulator ==="
echo ""

case "$1" in
    cpu)
        echo "Simulating high CPU load..."
        # Generate CPU load
        stress --cpu 4 --timeout 30 &
        echo "High CPU load started in background (will last 30 seconds)"
        ;;
    memory)
        echo "Simulating high memory usage..."
        # Allocate memory
        stress --vm 2 --vm-bytes 512M --timeout 30 &
        echo "High memory usage started in background (will last 30 seconds)"
        ;;
    disk)
        echo "Simulating high disk usage..."
        # Create large file
        dd if=/dev/zero of=/tmp/disk_test_file bs=1M count=500 2>/dev/null &
        echo "Disk fill started in background"
        ;;
    temperature)
        echo "Simulating high temperature..."
        # Create fake temperature file
        sudo mkdir -p /sys/class/thermal/thermal_zone0/
        echo "85000" | sudo tee /sys/class/thermal/thermal_zone0/temp > /dev/null
        echo "Temperature set to 85°C (simulated)"
        ;;
    network)
        echo "Simulating network failure..."
        # Bring interface down (requires root)
        sudo ip link set down eth0 2>/dev/null || echo "Cannot modify network interface (requires root)"
        echo "Network interface brought down (simulated)"
        ;;
    service)
        echo "Simulating service failure..."
        # Stop a service
        sudo systemctl stop sshd 2>/dev/null || echo "Cannot stop sshd (requires root)"
        echo "sshd service stopped (simulated)"
        ;;
    all)
        echo "Simulating multiple failures..."
        echo "Run individual tests for specific scenarios"
        ;;
    reset)
        echo "Resetting simulated conditions..."
        # Kill stress processes
        pkill -f stress 2>/dev/null || true
        # Restore network
        sudo ip link set up eth0 2>/dev/null || true
        # Restore temperature
        sudo rm -rf /sys/class/thermal/thermal_zone0/temp 2>/dev/null || true
        # Restart service
        sudo systemctl start sshd 2>/dev/null || true
        echo "Simulated conditions reset"
        ;;
    *)
        echo "Usage: $0 {cpu|memory|disk|temperature|network|service|all|reset}"
        echo ""
        echo "Options:"
        echo "  cpu        - Generate high CPU load"
        echo "  memory     - Generate high memory usage"
        echo "  disk       - Generate high disk usage"
        echo "  temperature - Simulate high temperature"
        echo "  network    - Simulate network failure"
        echo "  service    - Simulate service failure"
        echo "  all        - Show this help"
        echo "  reset      - Reset all simulated conditions"
        ;;
esac
