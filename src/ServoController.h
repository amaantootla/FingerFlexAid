#ifndef SERVO_CONTROLLER_H
#define SERVO_CONTROLLER_H

#include <ESP32Servo.h>
#include <vector>
#include <array>

class ServoController {
public:
    static constexpr int MIN_PULSE_WIDTH = 500;  // Microseconds
    static constexpr int MAX_PULSE_WIDTH = 2400; // Microseconds
    static constexpr int SERVO_FREQUENCY = 50;   // Hz

    explicit ServoController(const std::vector<int>& pins);
    ~ServoController();

    void updateServo(size_t index, int angle);
    void updateAllServos(const std::vector<int>& angles);
    void detachAll();

private:
    std::vector<Servo> servos;
};

#endif // SERVO_CONTROLLER_H
