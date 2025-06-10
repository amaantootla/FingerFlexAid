#include "MockServo.hpp"
#include <algorithm>
#include <cmath>

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

bool MockServo::setAngleLimits(uint16_t minAngle, uint16_t maxAngle)
{
    if (minAngle >= maxAngle || maxAngle > 180)
    {
        std::lock_guard<std::mutex> lock(errorMutex_);
        lastError_ = "Invalid angle limits: min=" + std::to_string(minAngle) + ", max=" + std::to_string(maxAngle);
        error_ = true;
        return false;
    }

    minAngle_ = minAngle;
    maxAngle_ = maxAngle;
    return true;
}

bool MockServo::setMaxSpeed(uint8_t maxSpeed)
{
    if (maxSpeed == 0 || maxSpeed > 100)
    {
        std::lock_guard<std::mutex> lock(errorMutex_);
        lastError_ = "Invalid max speed value: " + std::to_string(maxSpeed);
        isError_ = true;
        return false;
    }

    maxSpeed_ = maxSpeed;
    return true;
}

std::pair<uint16_t, uint16_t> MockServo::getAngleLimits() const
{
    return {minAngle_, maxAngle_};
}

uint8_t MockServo::getMaxSpeed() const
{
    return maxSpeed_;
}

void MockServo::simulateHardwareDelay(std::chrono::milliseconds delay)
{
    hardwareDelay_.store(delay);
}

void MockServo::simulateError(const std::string &error)
{
    std::lock_guard<std::mutex> lock(errorMutex_);
    lastError_ = error;
    isError_ = true;
}

void MockServo::clearError()
{
    std::lock_guard<std::mutex> lock(errorMutex_);
    lastError_.reset();
    isError_ = false;
}

void MockServo::updatePosition()
{
    while (!shouldStop_)
    {
        if (!isError_ && isMoving_)
        {
            int16_t angleDiff = static_cast<int16_t>(targetAngle_) - static_cast<int16_t>(currentAngle_);
            if (angleDiff != 0)
            {
                // Calculate base step size based on current speed
                double speedFactor = currentSpeed_ / static_cast<double>(maxSpeed_);

                // Use a non-linear movement curve that's faster in the middle and slower at the ends
                // This simulates the torque characteristics of a real servo
                double distanceFactor = std::min(1.0, std::abs(angleDiff) / 90.0);
                double movementFactor = 0.3 + (distanceFactor * 0.7); // Slower at start/end

                // Calculate final step size with speed and movement factors
                int16_t baseStep = std::max<int16_t>(1, static_cast<int16_t>(currentSpeed_ / 8));
                int16_t step = std::clamp<int16_t>(static_cast<int16_t>(baseStep * speedFactor * movementFactor), 1,
                                                   std::abs(angleDiff));

                if (angleDiff > 0)
                {
                    currentAngle_ += step;
                }
                else
                {
                    currentAngle_ -= step;
                }

                // Check if we're close enough to target to consider movement complete
                if (std::abs(angleDiff) <= 1)
                {
                    currentAngle_.store(targetAngle_.load());
                    isMoving_ = false;
                }
            }
            else
            {
                isMoving_ = false;
            }
        }
        std::this_thread::sleep_for(hardwareDelay_.load());
    }
}

bool MockServo::validateAngle(uint16_t angle) const
{
    return angle >= minAngle_ && angle <= maxAngle_;
}

bool MockServo::validateSpeed(uint8_t speed) const
{
    return speed <= maxSpeed_;
}

} // namespace FingerFlexAid
