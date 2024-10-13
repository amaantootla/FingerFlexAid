# FingerFlexAid

## Overview

FingerFlexAid is a hardware-assisted exercise system for hands. It uses an ESP32 microcontroller to control servo motors based on data from flex sensors attached to fingers. The system is controlled via a local server running Django, which communicates with the ESP32 over a REST API on the same LAN.

## Setup and Configuration

1. **LAN Communication**: The ESP32 and Django server must be on the same LAN for proper communication.

2. **Modify Pin Numbers**: 
   - Update the `hardware.h` file with the correct pin numbers for your board. The pin configuration will depend on the specific ESP32 model being used.

3. **Baud Rate**: 
   - Configure the baud rate as needed for your setup. NOTE 96000 IS TOO SLOW.

4. **Control Angles**: 
   - Configure target and assisted angles in both the server-side API and on the ESP32. NOTE please consult docs as "angles" mean have seemingly (??) different values depending on the servos being used.

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
