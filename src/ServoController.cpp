#include "ServoController.h"
#include <iostream>

ServoController::ServoController(const std::vector<int>& pins) {
    servos.reserve(pins.size());
    
    for (auto pin : pins) {
        Servo servo;
        servo.setPeriodHertz(SERVO_FREQUENCY);
        servo.attach(pin, MIN_PULSE_WIDTH, MAX_PULSE_WIDTH);
        servos.push_back(std::move(servo));
    }
}

ServoController::~ServoController() {
    detachAll();
}

void ServoController::updateServo(size_t index, int angle) {
    if (index < servos.size()) {
        servos[index].write(angle);
    }
}

void ServoController::updateAllServos(const std::vector<int>& angles) {
    for (size_t i = 0; i < angles.size() && i < servos.size(); ++i) {
        updateServo(i, angles[i]);
    }
}

void ServoController::detachAll() {
    for (auto& servo : servos) {
        servo.detach();
    }
}
