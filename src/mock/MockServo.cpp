#include "MockServo.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <thread>

using namespace FingerFlexAid;
using namespace std::chrono_literals;

MockServo::MockServo(const std::string &id) : id_(id)
{
    updateThread_ = std::thread(&MockServo::updateAngle, this);
}

MockServo::~MockServo()
{
    shouldStop_ = true;
    if (updateThread_.joinable())
    {
        updateThread_.join();
    }
}

void MockServo::setAngle(double angle)
{
    setAngleImpl(angle);
}

bool MockServo::setAngleChecked(double angle)
{
    return setAngleImpl(angle);
}

double MockServo::getAngle() const
{
    return currentAngle_.load();
}

void MockServo::setSpeed(double speed)
{
    setSpeedImpl(speed);
}

bool MockServo::setSpeedChecked(double speed)
{
    return setSpeedImpl(speed);
}

double MockServo::getSpeed() const
{
    return currentSpeed_.load();
}

bool MockServo::isMoving() const
{
    return isMoving_.load();
}

void MockServo::simulateError(bool simulate)
{
    std::lock_guard<std::mutex> lock(errorMutex_);
    error_.store(simulate);
    if (!simulate)
    {
        lastError_.clear();
    }
}

bool MockServo::hasError() const
{
    return error_.load();
}

bool MockServo::setAngleLimits(double min, double max)
{
    if (min >= max || min < 0 || max > 180)
    {
        simulateError(true);
        return false;
    }
    minAngle_.store(min);
    maxAngle_.store(max);
    return true;
}

bool MockServo::setMaxSpeed(double speed)
{
    if (speed <= 0 || speed > 100)
    {
        simulateError(true);
        return false;
    }
    maxSpeed_.store(speed);
    return true;
}

bool MockServo::stop()
{
    if (hasError())
        return false;
    isMoving_ = false;
    return true;
}

bool MockServo::emergencyStop()
{
    isMoving_ = false;
    simulateError("Emergency stop activated");
    return true;
}

void MockServo::clearError()
{
    std::lock_guard<std::mutex> lock(errorMutex_);
    error_ = false;
    lastError_.clear();
}

void MockServo::simulateError(const std::string &errorMsg)
{
    std::lock_guard<std::mutex> lock(errorMutex_);
    error_ = true;
    lastError_ = errorMsg;
}

void MockServo::simulateHardwareDelay(std::chrono::milliseconds delay)
{
    hardwareDelay_ = delay;
}

std::optional<std::string> MockServo::getLastError() const
{
    std::lock_guard<std::mutex> lock(errorMutex_);
    return lastError_.empty() ? std::nullopt : std::optional<std::string>(lastError_);
}

void MockServo::updateAngle()
{
    while (!shouldStop_)
    {
        if (isMoving_ && !hasError())
        {
            // Non-linear movement curve for more realistic simulation
            double current = currentAngle_.load();
            double target = targetAngle_.load();
            double diff = target - current;
            double absDiff = std::abs(diff);

            if (absDiff > 0.1) // Only move if difference is significant
            {
                // Calculate step size based on current speed and difference
                double speed = currentSpeed_.load();
                double step = std::min(absDiff, speed * 0.1);
                // Apply non-linear curve (slower at start/end, faster in middle)
                step *= std::sin(M_PI * (1.0 - absDiff / 180.0));

                if (diff > 0)
                    currentAngle_.store(current + step);
                else
                    currentAngle_.store(current - step);
            }
            else
            {
                currentAngle_.store(target);
                isMoving_ = false;
            }
        }

        // Gradually update speed
        double currentSpeed = currentSpeed_.load();
        double targetSpeed = targetSpeed_.load();
        double speedDiff = targetSpeed - currentSpeed;
        if (std::abs(speedDiff) > 0.1)
        {
            double step = std::copysign(std::min(std::abs(speedDiff), 5.0), speedDiff);
            currentSpeed_.store(currentSpeed + step);
        }
        else
        {
            currentSpeed_.store(targetSpeed);
        }

        std::this_thread::sleep_for(20ms + hardwareDelay_.load());
    }
}

bool MockServo::setAngleImpl(double angle)
{
    if (hasError())
        return false;
    double min = minAngle_.load();
    double max = maxAngle_.load();
    if (angle < min || angle > max)
        return false;
    angle = std::clamp(angle, min, max);
    targetAngle_.store(angle);
    isMoving_ = true;
    return true;
}

bool MockServo::setSpeedImpl(double speed)
{
    if (hasError())
        return false;
    double max = maxSpeed_.load();
    if (speed < 0.0 || speed > max)
        return false;
    speed = std::clamp(speed, 0.0, max);
    targetSpeed_.store(speed);
    return true;
}
