#pragma once
#include "models/Motor.hpp"
#include "models/Servo.hpp"
#include <memory>
#include <mutex>
#include <vector>

namespace FingerFlexAid
{

class GloveState
{
  public:
    GloveState();
    void addMotor(std::shared_ptr<Motor> motor);
    void addServo(std::shared_ptr<Servo> servo);
    void update();         // update all device states
    void reset();          // reset all device states
    bool hasError() const; // check for any error
                           // (future: add more safety/coordination methods)

  private:
    mutable std::mutex mtx;
    std::vector<std::shared_ptr<Motor>> motors;
    std::vector<std::shared_ptr<Servo>> servos;
};

} // namespace FingerFlexAid
