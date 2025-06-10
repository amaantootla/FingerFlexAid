#pragma once

#include <cstdint>
#include <optional>
#include <string>

namespace FingerFlexAid
{

class ServoController
{
  public:
    virtual ~ServoController() = default;

    virtual bool setAngle(uint16_t angle) = 0; // Angle in degrees (0-180)
    virtual bool setSpeed(uint8_t speed) = 0;  // Speed (0-100)
    virtual bool stop() = 0;
    virtual bool emergencyStop() = 0;

    virtual uint16_t getCurrentAngle() const = 0;
    virtual uint8_t getCurrentSpeed() const = 0;
    virtual bool isMoving() const = 0;
    virtual bool isError() const = 0;
    virtual std::optional<std::string> getLastError() const = 0;

    virtual bool setAngleLimits(uint16_t minAngle, uint16_t maxAngle) = 0;
    virtual bool setMaxSpeed(uint8_t maxSpeed) = 0;
    virtual std::pair<uint16_t, uint16_t> getAngleLimits() const = 0;
    virtual uint8_t getMaxSpeed() const = 0;

  protected:
    ServoController() = default;
    ServoController(const ServoController &) = default;
    ServoController &operator=(const ServoController &) = default;
    ServoController(ServoController &&) = default;
    ServoController &operator=(ServoController &&) = default;
};

}
