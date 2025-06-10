# FingerFlexAid

A C++ software system for controlling rehabilitation gloves with motors and servos. This project focuses on the software implementation with mock hardware interfaces for testing and development.

## Requirements

- C++23 compatible compiler
- CMake 3.20 or higher
- Google Test (GTest)
- Build tools (make, ninja, or your preferred build system)

## Building the Project

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

## Project Structure

- `src/core/` - Core interfaces and device management
- `src/models/` - Data models and state management
- `src/mock/` - Mock hardware implementations
- `src/utils/` - Utility classes and helpers
- `tests/` - Unit and integration tests

## Development

This project uses modern C++ features and follows these principles:
- KISS (Keep It Simple, Stupid)
- DRY (Don't Repeat Yourself)
- YAGNI (You Aren't Gonna Need It)
- SOLID principles
- camelCase naming convention

## Testing

The project uses Google Test for unit testing. Run tests using:

```bash
cd build
ctest
```

## License

[Add your license here]

## Contributing

[Add contribution guidelines here] 
