# FingerFlexAid

A C++ software system for controlling rehabilitation gloves with motors and servos, designed to assist in hand rehabilitation therapy. The system consists of two main components:

1. A core C++ library that provides the control logic, state management, and device interfaces
2. An ESP32 microcontroller bridge that interfaces with the physical hardware (motors, servos, sensors)

The project is structured to allow development and testing of the control logic independently of the hardware, using mock implementations, while providing a clear path to deployment on ESP32 microcontrollers.

## Features

- Precise control of finger movement through motor/servo actuation
- Configurable exercise routines and movement patterns
- Real-time monitoring of finger positions and forces
- Mock hardware interfaces for development and testing
- ESP32 integration for physical hardware control
- Extensible architecture for different glove configurations

## Requirements

### Development Environment
- C++23 compatible compiler
- CMake 3.20 or higher
- Google Test (GTest)
- Build tools (make, ninja, or your preferred build system)

### Hardware Requirements (for ESP32 deployment)
- ESP32 microcontroller (ESP32-WROOM-32 or compatible)
- Motor drivers (compatible with your specific motors)
- Servo motors for finger actuation
- Force sensors (optional, for feedback)
- Power supply (5V/12V depending on motor requirements)
- USB connection for programming and debugging

## Building the Project

### Core Library
```bash
# Create build directory
mkdir build && cd build

# Configure with CMake
cmake ..

# Build the project
cmake --build .

# Run tests
ctest
```

### ESP32 Bridge
The ESP32 code needs to be loaded and then used with the Arduino framework. To build and deploy:

1. Install Arduino IDE or PlatformIO
2. Open the project in your preferred environment
3. Install required libraries
4. Build and upload to your ESP32

## Project Structure

- `src/core/` - Core interfaces and device management
- `src/models/` - Data models and state management
- `src/mock/` - Mock hardware implementations
- `src/utils/` - Utility classes and helpers
- `tests/` - Unit and integration tests

## Testing

The project uses Google Test for unit testing. Run tests using:

```bash
cd build
ctest
```

## Hardware Integration

The ESP32 bridge provides the following hardware interfaces:
- Motor control via PWM and motor drivers
- Servo control using ESP32's PWM capabilities
- Sensor reading (force, position) via ADC
- Serial communication with host system
- Power management and safety features
