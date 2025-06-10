#include "MockServo.hpp"
#include <algorithm>
#include <cmath>
#include <thread>

namespace FingerFlexAid
{

MockServo::MockServo(const std::string &id) : id_(id), updateThread_([this]() { updateAngle(); })
{
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
    if (error_)
    {
        return;
    }
    targetAngle_ = angle;
    if (angle != currentAngle_)
    {
        isMoving_ = true;
    }
}

double MockServo::getAngle() const
{
    return currentAngle_;
}

void MockServo::setSpeed(double speed)
{
    if (error_)
    {
        return;
    }
    targetSpeed_ = speed;
    currentSpeed_ = speed;
}

double MockServo::getSpeed() const
{
    return currentSpeed_;
}

bool MockServo::isMoving() const
{
    return isMoving_;
}

void MockServo::simulateError(bool simulate)
{
    std::lock_guard<std::mutex> lock(errorMutex_);
    error_ = simulate;
    lastError_ = simulate ? "Simulated error" : "";
}

bool MockServo::hasError() const
{
    return error_;
}

void MockServo::updateAngle()
{
    while (!shouldStop_)
    {
        if (!error_ && isMoving_)
        {
            double angleDiff = targetAngle_ - currentAngle_;
            if (std::abs(angleDiff) > 0.01)
            {
                double step = std::clamp(currentSpeed_ / 8.0, 0.1, std::abs(angleDiff));
                if (angleDiff > 0)
                {
                    currentAngle_ += step;
                }
                else
                {
                    currentAngle_ -= step;
                }
                if (std::abs(targetAngle_ - currentAngle_) <= 0.01)
                {
                    currentAngle_ = targetAngle_;
                    isMoving_ = false;
                }
            }
            else
            {
                isMoving_ = false;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
}

} // namespace FingerFlexAid
