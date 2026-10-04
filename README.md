# Embedded Linux Device Health Monitor & Auto-Recovery Agent

A C/C++ background service that continuously monitors Linux system health and automatically recovers from failures.

## Features

- Monitors CPU usage, RAM, disk space, temperature, network status, and critical services
- Configurable thresholds via JSON configuration
- Automatic recovery actions (restart services, kill processes)
- Systemd service integration
- Comprehensive logging

## Build

```bash
mkdir build && cd build
cmake ..
make
```

## Install

```bash
sudo ./scripts/install.sh
```

## Uninstall

```bash
sudo ./scripts/uninstall.sh
```

## Configuration

Edit `config/health_monitor.json` to customize monitoring thresholds and recovery actions.
