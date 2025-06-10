#pragma once

#include "DeviceManager.hpp"
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace FingerFlexAid
{

class DeviceManagerImpl : public DeviceManager
{
  public:
    DeviceManagerImpl();
    ~DeviceManagerImpl() override;

    bool registerMotor(const std::string &id, std::shared_ptr<MotorController> motor) override;
    bool registerServo(const std::string &id, std::shared_ptr<ServoController> servo) override;
    bool unregisterDevice(const std::string &id) override;

    std::shared_ptr<MotorController> getMotor(const std::string &id) const override;
    std::shared_ptr<ServoController> getServo(const std::string &id) const override;
    std::vector<std::string> getMotorIds() const override;
    std::vector<std::string> getServoIds() const override;

    bool initializeAll() override;
    bool shutdownAll() override;
    bool emergencyStopAll() override;
    bool isAnyDeviceMoving() const override;
    bool isAnyDeviceInError() const override;
    std::vector<std::string> getDevicesInError() const override;

    size_t getMotorCount() const override;
    size_t getServoCount() const override;
    bool isInitialized() const override;

  private:
    mutable std::mutex mutex_;
    std::unordered_map<std::string, std::shared_ptr<MotorController>> motors_;
    std::unordered_map<std::string, std::shared_ptr<ServoController>> servos_;
    bool initialized_ = false;
};

} // namespace FingerFlexAid
