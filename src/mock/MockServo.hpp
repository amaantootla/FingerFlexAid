#pragma once

#include "../models/Servo.hpp"
#include <atomic>
#include <chrono>
#include <mutex>
#include <optional>
#include <string>
#include <thread>

namespace FingerFlexAid
{

class MockServo : public Servo
{
  public:
    explicit MockServo(const std::string &id = "mock_servo");
    ~MockServo() override;

    // Base Servo interface implementation
    void setAngle(double angle) override;
    double getAngle() const override;
    void setSpeed(double speed) override;
    double getSpeed() const override;
    bool isMoving() const override;
    void simulateError(bool simulate) override;
    bool hasError() const override;

    // Additional mock-specific methods
    std::string getId() const
    {
        return id_;
    }
    double getCurrentAngle() const
    {
        return currentAngle_;
    }
    double getCurrentSpeed() const
    {
        return currentSpeed_;
    }
    double getMaxSpeed() const
    {
        return maxSpeed_;
    }
    std::pair<double, double> getAngleLimits() const
    {
        return {minAngle_, maxAngle_};
    }
    bool setAngleLimits(double min, double max);
    bool setMaxSpeed(double speed);
    bool stop();
    bool emergencyStop();
    void clearError();
    void simulateError(const std::string &errorMsg);
    void simulateHardwareDelay(std::chrono::milliseconds delay);
    std::optional<std::string> getLastError() const;
    bool isError() const
    {
        return hasError();
    }

    // Overloads for test compatibility
    bool setAngle(bool angle); // will be implemented as setAngle(double) and return bool
    bool setSpeed(bool speed); // will be implemented as setSpeed(double) and return bool
    bool setAngleChecked(double angle);
    bool setSpeedChecked(double speed);

  private:
    void updateAngle();
    bool setAngleImpl(double angle);
    bool setSpeedImpl(double speed);
    const std::string id_;
    std::atomic<double> currentAngle_{90.0};
    std::atomic<double> currentSpeed_{50.0};
    std::atomic<double> targetAngle_{90.0};
    std::atomic<double> targetSpeed_{50.0};
    std::atomic<double> maxSpeed_{100.0};
    std::atomic<double> minAngle_{0.0};
    std::atomic<double> maxAngle_{180.0};
    std::atomic<bool> isMoving_{false};
    std::atomic<bool> error_{false};
    mutable std::mutex errorMutex_;
    std::string lastError_;
    std::thread updateThread_;
    std::atomic<bool> shouldStop_{false};
    std::atomic<std::chrono::milliseconds> hardwareDelay_{std::chrono::milliseconds{0}};
};

} // namespace FingerFlexAid
