#pragma once

#include "../core/MotorController.hpp"
#include "models/Motor.hpp"
#include <atomic>
#include <chrono>
#include <mutex>
#include <thread>

namespace FingerFlexAid
{

class MockMotor : public Motor
{
  public:
    explicit MockMotor(const std::string &id = "mock_motor");
    ~MockMotor();

    bool setSpeed(int16_t speed);
    bool setPosition(int32_t position);
    bool stop();
    bool emergencyStop();

    int16_t getCurrentSpeed() const;
    int32_t getCurrentPosition() const;
    bool isMoving() const;
    bool isError() const;
    std::optional<std::string> getLastError() const;

    bool setMaxSpeed(int16_t maxSpeed);
    bool setAcceleration(uint16_t acceleration);
    int16_t getMaxSpeed() const;
    uint16_t getAcceleration() const;

    void simulateHardwareDelay(std::chrono::milliseconds delay);
    void simulateError(const std::string &error);
    void clearError();
    std::string getId() const
    {
        return id_;
    }

  private:
    void updatePosition();
    bool validateSpeed(int16_t speed) const;
    bool validatePosition(int32_t position) const;

    const std::string id_;
    std::atomic<int16_t> currentSpeed_{0};
    std::atomic<int32_t> currentPosition_{0};
    std::atomic<int16_t> targetSpeed_{0};
    std::atomic<int32_t> targetPosition_{0};
    std::atomic<int16_t> maxSpeed_{1000};
    std::atomic<uint16_t> acceleration_{1000};
    std::atomic<bool> isMoving_{false};
    std::atomic<bool> isError_{false};
    std::atomic<std::chrono::milliseconds> hardwareDelay_{std::chrono::milliseconds(10)};

    mutable std::mutex errorMutex_;
    std::optional<std::string> lastError_;

    std::thread updateThread_;
    std::atomic<bool> shouldStop_{false};
    mutable std::mutex stateMutex_;
};

} // namespace FingerFlexAid
