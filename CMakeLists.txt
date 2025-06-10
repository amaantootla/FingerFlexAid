cmake_minimum_required(VERSION 3.20)
project(FingerFlexAid VERSION 0.1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable testing
enable_testing()

# Set compiler flags
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
elseif(MSVC)
    add_compile_options(/W4 /WX)
endif()

# Find required packages
find_package(GTest REQUIRED)

# Create a library target for the core functionality
add_library(${PROJECT_NAME}_lib
    src/mock/MockMotor.cpp
    src/mock/MockServo.cpp
    src/models/Motor.cpp
    src/models/Servo.cpp
    src/models/GloveState.cpp
)

# Add include directories for the library
target_include_directories(${PROJECT_NAME}_lib
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Create the main executable
add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_lib)

# Create the test executable
add_executable(${PROJECT_NAME}_tests
    tests/MotorTests.cpp
    tests/ServoTests.cpp
)

# Link the test executable with the library and GTest
target_link_libraries(${PROJECT_NAME}_tests
    PRIVATE
        ${PROJECT_NAME}_lib
        GTest::GTest
        GTest::Main
)

# Add tests
add_test(NAME ${PROJECT_NAME}_tests COMMAND ${PROJECT_NAME}_tests)

# Installation
install(TARGETS ${PROJECT_NAME} ${PROJECT_NAME}_lib
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

# Set output directories
set_target_properties(${PROJECT_NAME} ${PROJECT_NAME}_lib ${PROJECT_NAME}_tests
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
) 
