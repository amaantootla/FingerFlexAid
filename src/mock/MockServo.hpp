#pragma once

#include "../core/ServoController.hpp"
#include <atomic>
#include <chrono>
#include <mutex>
#include <thread>

namespace FingerFlexAid
{

class MockServo : public ServoController
{
  public:
    explicit MockServo(const std::string &id = "mock_servo");
    ~MockServo() override;

    bool setAngle(uint16_t angle) override;
    bool setSpeed(uint8_t speed) override;
    bool stop() override;
    bool emergencyStop() override;

    uint16_t getCurrentAngle() const override;
    uint8_t getCurrentSpeed() const override;
    bool isMoving() const override;
    bool isError() const override;
    std::optional<std::string> getLastError() const override;

    bool setAngleLimits(uint16_t minAngle, uint16_t maxAngle) override;
    bool setMaxSpeed(uint8_t maxSpeed) override;
    std::pair<uint16_t, uint16_t> getAngleLimits() const override;
    uint8_t getMaxSpeed() const override;

    void simulateHardwareDelay(std::chrono::milliseconds delay);
    void simulateError(const std::string &error);
    void clearError();
    std::string getId() const
    {
        return id_;
    }

  private:
    void updatePosition();
    bool validateAngle(uint16_t angle) const;
    bool validateSpeed(uint8_t speed) const;

    const std::string id_;
    std::atomic<uint16_t> currentAngle_{90}; // Start at middle position
    std::atomic<uint8_t> currentSpeed_{50};  // Start at medium speed
    std::atomic<uint16_t> targetAngle_{90};
    std::atomic<uint8_t> targetSpeed_{50};
    std::atomic<uint16_t> minAngle_{0};
    std::atomic<uint16_t> maxAngle_{180};
    std::atomic<uint8_t> maxSpeed_{100};
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
