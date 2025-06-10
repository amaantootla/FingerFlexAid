#include "models/Servo.hpp"
#include <algorithm>

namespace FingerFlexAid
{

ServoImpl::ServoImpl(double minAngle, double maxAngle, double defaultSpeed, const std::string &name)
    : _minAngle(minAngle), _maxAngle(maxAngle), _currentAngle(minAngle), _currentSpeed(defaultSpeed), _moving(false),
      _simulateError(false), _hasError(false), _name(name)
{
}

void ServoImpl::setAngle(double angle)
{
    std::lock_guard<std::mutex> lock(mtx);
    if (_simulateError)
    {
        _hasError = true;
        return;
    }
    double clamped = std::clamp(angle, _minAngle, _maxAngle);
    _currentAngle = clamped;
    _moving = (_currentAngle != angle); // simplistic: moving if clamped
}

double ServoImpl::getAngle() const
{
    std::lock_guard<std::mutex> lock(mtx);
    return _currentAngle;
}

void ServoImpl::setSpeed(double speed)
{
    std::lock_guard<std::mutex> lock(mtx);
    if (_simulateError)
    {
        _hasError = true;
        return;
    }
    _currentSpeed = std::max(0.0, speed);
    _moving = (_currentSpeed > 0.0);
}

double ServoImpl::getSpeed() const
{
    std::lock_guard<std::mutex> lock(mtx);
    return _currentSpeed;
}

bool ServoImpl::isMoving() const
{
    return _moving.load();
}

void ServoImpl::simulateError(bool simulate)
{
    _simulateError = simulate;
    if (!simulate)
        _hasError = false;
}

bool ServoImpl::hasError() const
{
    return _hasError.load();
}

} // namespace FingerFlexAid
