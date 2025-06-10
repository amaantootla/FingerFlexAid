#pragma once

#include "MotorController.hpp"
#include "ServoController.hpp"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace FingerFlexAid
{

class DeviceManager
{
  public:
    virtual ~DeviceManager() = default;

    virtual bool registerMotor(const std::string &id, std::shared_ptr<MotorController> motor) = 0;
    virtual bool registerServo(const std::string &id, std::shared_ptr<ServoController> servo) = 0;
    virtual bool unregisterDevice(const std::string &id) = 0;

    virtual std::shared_ptr<MotorController> getMotor(const std::string &id) const = 0;
    virtual std::shared_ptr<ServoController> getServo(const std::string &id) const = 0;
    virtual std::vector<std::string> getMotorIds() const = 0;
    virtual std::vector<std::string> getServoIds() const = 0;

    virtual bool initializeAll() = 0;
    virtual bool shutdownAll() = 0;
    virtual bool emergencyStopAll() = 0;
    virtual bool isAnyDeviceMoving() const = 0;
    virtual bool isAnyDeviceInError() const = 0;
    virtual std::vector<std::string> getDevicesInError() const = 0;

    virtual size_t getMotorCount() const = 0;
    virtual size_t getServoCount() const = 0;
    virtual bool isInitialized() const = 0;

  protected:
    DeviceManager() = default;
    DeviceManager(const DeviceManager &) = default;
    DeviceManager &operator=(const DeviceManager &) = default;
    DeviceManager(DeviceManager &&) = default;
    DeviceManager &operator=(DeviceManager &&) = default;
};

}
