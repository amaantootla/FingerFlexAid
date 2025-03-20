# FingerFlexAid

This project is a hardware-assisted system designed to help with hand exercises. It uses an ESP32 microcontroller to control servo motors based on data from flex sensors attached to fingers. The system communicates with a Django-based server over a local network to provide real-time control and feedback.

## How to Install and Run

1. Make sure you have an ESP32 board and the necessary flex sensors and servos connected as described in the hardware documentation.
2. Clone this repository to your local machine.
3. Install PlatformIO in Visual Studio Code for building and uploading the firmware to the ESP32.
4. Update the `hardware.h` file with the correct pin numbers for your ESP32 board and the servo/flex sensor setup.
5. Build and upload the firmware to the ESP32 using PlatformIO.
6. Start the Django server on your local machine and ensure the ESP32 is connected to the same WiFi network.

## Main Features

- Reads tilt data from flex sensors and calculates the appropriate servo angles using a kinematics algorithm.
- Controls servo motors to assist with finger movements based on the calculated angles.
- Communicates with a Django server over a REST API for real-time control and monitoring.

## Design Decisions

1. **Servo Control**: We use the ESP32Servo library to handle PWM signals for the servos. We considered using a custom PWM implementation but decided the library was more reliable and easier to maintain.
2. **Kinematics Algorithm**: The current implementation uses a simple trigonometric model to calculate servo angles. We explored more complex models but kept it simple to ensure real-time performance on the ESP32.

## Usage Notes

- Make sure the ESP32 and Django server are on the same local network for proper communication.
- The servo angles can be configured both in the ESP32 firmware and through the Django server's API.
- If you encounter issues with servo jitter, check the power supply and ensure the pulse width settings in `ServoController.h` match your servo specifications.

This project is still a work in progress, so feel free to contribute or suggest improvements!

## TODO

Tasks for the future, feel free to make a PR :).

1. **Improve Web UI for Direct Angle Modification**:
   - Implement a better user interface for real-time control of angles, allowing users to directly input and modify target and assisted angles through the web application.
   - Provide live feedback in the UI, reflecting changes made to the angles as the ESP32 responds in real time.

2. **Transition to Django REST Framework (DRF)**:
   - Refactor the existing codebase to use Django REST Framework (DRF) instead of the current MVC-based controller and view setup.
   - Implement full API endpoints using DRF for better scalability, easier maintenance, and better adherence to RESTful principles.

3. **Add Data Logging and Visualization**:
   - Implement data logging functionality to store sensor readings and angle positions.
   - Provide a visual representation of progress over time, helping users track their rehabilitation exercises more effectively.
