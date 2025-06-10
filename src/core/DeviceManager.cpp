#include "DeviceManagerImpl.hpp"

namespace FingerFlexAid
{

DeviceManagerImpl::DeviceManagerImpl() = default;
DeviceManagerImpl::~DeviceManagerImpl() = default;

bool DeviceManagerImpl::registerMotor(const std::string &id, std::shared_ptr<MotorController> motor)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!motor || motors_.count(id))
        return false;
    motors_[id] = motor;
    return true;
}

bool DeviceManagerImpl::registerServo(const std::string &id, std::shared_ptr<ServoController> servo)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!servo || servos_.count(id))
        return false;
    servos_[id] = servo;
    return true;
}

bool DeviceManagerImpl::unregisterDevice(const std::string &id)
{
    std::lock_guard<std::mutex> lock(mutex_);
    size_t removed = motors_.erase(id);
    removed += servos_.erase(id);
    return removed > 0;
}

std::shared_ptr<MotorController> DeviceManagerImpl::getMotor(const std::string &id) const
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = motors_.find(id);
    return it != motors_.end() ? it->second : nullptr;
}

std::shared_ptr<ServoController> DeviceManagerImpl::getServo(const std::string &id) const
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = servos_.find(id);
    return it != servos_.end() ? it->second : nullptr;
}

std::vector<std::string> DeviceManagerImpl::getMotorIds() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<std::string> ids;
    for (const auto &kv : motors_)
        ids.push_back(kv.first);
    return ids;
}

std::vector<std::string> DeviceManagerImpl::getServoIds() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<std::string> ids;
    for (const auto &kv : servos_)
        ids.push_back(kv.first);
    return ids;
}

bool DeviceManagerImpl::initializeAll()
{
    std::lock_guard<std::mutex> lock(mutex_);
    initialized_ = true;
    return true;
}

bool DeviceManagerImpl::shutdownAll()
{
    std::lock_guard<std::mutex> lock(mutex_);
    initialized_ = false;
    return true;
}

bool DeviceManagerImpl::emergencyStopAll()
{
    std::lock_guard<std::mutex> lock(mutex_);
    bool any = false;
    for (auto &kv : motors_)
        any |= kv.second->emergencyStop();
    for (auto &kv : servos_)
        any |= kv.second->emergencyStop();
    return any;
}

bool DeviceManagerImpl::isAnyDeviceMoving() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    for (const auto &kv : motors_)
        if (kv.second->isMoving())
            return true;
    for (const auto &kv : servos_)
        if (kv.second->isMoving())
            return true;
    return false;
}

bool DeviceManagerImpl::isAnyDeviceInError() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    for (const auto &kv : motors_)
        if (kv.second->isError())
            return true;
    for (const auto &kv : servos_)
        if (kv.second->isError())
            return true;
    return false;
}

std::vector<std::string> DeviceManagerImpl::getDevicesInError() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<std::string> ids;
    for (const auto &kv : motors_)
        if (kv.second->isError())
            ids.push_back(kv.first);
    for (const auto &kv : servos_)
        if (kv.second->isError())
            ids.push_back(kv.first);
    return ids;
}

size_t DeviceManagerImpl::getMotorCount() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return motors_.size();
}

size_t DeviceManagerImpl::getServoCount() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return servos_.size();
}

bool DeviceManagerImpl::isInitialized() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return initialized_;
}

} // namespace FingerFlexAid
