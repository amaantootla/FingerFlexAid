#pragma once

#include "../core/ServoController.hpp"
#include "models/Servo.hpp"
#include <atomic>
#include <chrono>
#include <mutex>
#include <thread>

namespace FingerFlexAid
{

class MockServo : public Servo
{
  public:
    explicit MockServo(const std::string &id = "mock_servo");
    ~MockServo() override;

    void setAngle(double angle) override;
    double getAngle() const override;
    void setSpeed(double speed) override;
    double getSpeed() const override;
    bool isMoving() const override;
    void simulateError(bool simulate) override;
    bool hasError() const override;

    std::string getId() const
    {
        return id_;
    }

  private:
    void updateAngle();
    const std::string id_;
    std::atomic<double> currentAngle_{90.0};
    std::atomic<double> currentSpeed_{50.0};
    std::atomic<double> targetAngle_{90.0};
    std::atomic<double> targetSpeed_{50.0};
    std::atomic<bool> isMoving_{false};
    std::atomic<bool> error_{false};
    mutable std::mutex errorMutex_;
    std::string lastError_;
    std::thread updateThread_;
    std::atomic<bool> shouldStop_{false};
};

} // namespace FingerFlexAid
