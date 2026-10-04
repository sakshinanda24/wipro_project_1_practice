cmake_minimum_required(VERSION 3.10)
project(DeviceHealthMonitor VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Source files
set(SOURCES
    src/main.cpp
    src/cpu_monitor.cpp
    src/memory_monitor.cpp
    src/disk_monitor.cpp
    src/temperature_monitor.cpp
    src/network_monitor.cpp
    src/service_monitor.cpp
    src/logger.cpp
    src/config_manager.cpp
)

# Create executable
add_executable(device_health_monitor ${SOURCES})

# Link libraries
find_package(Threads REQUIRED)
target_link_libraries(device_health_monitor
    Threads::Threads
    pthread
)

# Install executable
install(TARGETS device_health_monitor DESTINATION /usr/local/bin)

# Install config
install(FILES config/health_monitor.json DESTINATION /etc/device_health_monitor/)

# Install systemd service
install(FILES systemd/device_health_monitor.service DESTINATION /etc/systemd/system/)
