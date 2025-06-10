#pragma once

#include <cstdint>
#include <optional>
#include <string>

namespace FingerFlexAid
{

class MotorController
{
  public:
    virtual ~MotorController() = default;

    virtual bool setSpeed(int16_t speed) = 0;       // Speed in RPM, negative for reverse
    virtual bool setPosition(int32_t position) = 0; // Position in steps
    virtual bool stop() = 0;
    virtual bool emergencyStop() = 0;

    virtual int16_t getCurrentSpeed() const = 0;
    virtual int32_t getCurrentPosition() const = 0;
    virtual bool isMoving() const = 0;
    virtual bool isError() const = 0;
    virtual std::optional<std::string> getLastError() const = 0;

    virtual bool setMaxSpeed(int16_t maxSpeed) = 0;
    virtual bool setAcceleration(uint16_t acceleration) = 0;
    virtual int16_t getMaxSpeed() const = 0;
    virtual uint16_t getAcceleration() const = 0;

  protected:
    MotorController() = default;
    MotorController(const MotorController &) = default;
    MotorController &operator=(const MotorController &) = default;
    MotorController(MotorController &&) = default;
    MotorController &operator=(MotorController &&) = default;
};

}
