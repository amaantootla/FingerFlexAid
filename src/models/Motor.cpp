#include "Motor.hpp"
#include <algorithm>

namespace FingerFlexAid
{

Motor::Motor(const std::string &id, double maxSpeed, double maxTorque)
    : id_(id), maxSpeed_(maxSpeed), maxTorque_(maxTorque), speed_(0), position_(0), moving_(false), error_(false),
      errorMsg_("")
{
}

Motor::~Motor() = default;

void Motor::setSpeed(double speed)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (error_)
        return;
    speed_ = std::clamp(speed, -maxSpeed_, maxSpeed_);
    moving_ = (speed != 0);
}

double Motor::getSpeed() const
{
    return speed_.load();
}

void Motor::setPosition(double position)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (error_)
        return;
    position_ = position;
}

double Motor::getPosition() const
{
    return position_.load();
}

double Motor::getMaxSpeed() const
{
    return maxSpeed_;
}

double Motor::getMaxTorque() const
{
    return maxTorque_;
}

std::string Motor::getId() const
{
    return id_;
}

bool Motor::isMoving() const
{
    return moving_.load();
}

bool Motor::isError() const
{
    return error_.load();
}

std::string Motor::getErrorMessage() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return errorMsg_;
}

void Motor::clearError()
{
    std::lock_guard<std::mutex> lock(mutex_);
    error_ = false;
    errorMsg_.clear();
}

void Motor::simulateError(const std::string &msg)
{
    std::lock_guard<std::mutex> lock(mutex_);
    error_ = true;
    errorMsg_ = msg;
    speed_ = 0;
    moving_ = false;
}

} // namespace FingerFlexAid
