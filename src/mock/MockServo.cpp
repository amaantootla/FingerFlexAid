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

bool MockServo::setAngle(uint16_t angle)
{
    if (isError_)
    {
        return false;
    }

    if (!validateAngle(angle))
    {
        std::lock_guard<std::mutex> lock(errorMutex_);
        lastError_ = "Invalid angle value: " + std::to_string(angle);
        isError_ = true;
        return false;
    }

    targetAngle_ = angle;
    if (angle != currentAngle_)
    {
        isMoving_ = true;
    }
    return true;
}

bool MockServo::setSpeed(uint8_t speed)
{
    if (isError_)
    {
        return false;
    }

    if (!validateSpeed(speed))
    {
        std::lock_guard<std::mutex> lock(errorMutex_);
        lastError_ = "Invalid speed value: " + std::to_string(speed);
        isError_ = true;
        return false;
    }

    targetSpeed_ = speed;
    currentSpeed_ = speed;
    return true;
}

bool MockServo::stop()
{
    if (isError_)
    {
        return false;
    }

    isMoving_ = false;
    return true;
}

bool MockServo::emergencyStop()
{
    isMoving_ = false;
    isError_ = true;

    std::lock_guard<std::mutex> lock(errorMutex_);
    lastError_ = "Emergency stop activated";
    return true;
}

uint16_t MockServo::getCurrentAngle() const
{
    return currentAngle_;
}

uint8_t MockServo::getCurrentSpeed() const
{
    return currentSpeed_;
}

bool MockServo::isMoving() const
{
    return isMoving_;
}

bool MockServo::isError() const
{
    return isError_;
}

std::optional<std::string> MockServo::getLastError() const
{
    std::lock_guard<std::mutex> lock(errorMutex_);
    return lastError_;
}

bool MockServo::setAngleLimits(uint16_t minAngle, uint16_t maxAngle)
{
    if (minAngle >= maxAngle || maxAngle > 180)
    {
        std::lock_guard<std::mutex> lock(errorMutex_);
        lastError_ = "Invalid angle limits: min=" + std::to_string(minAngle) + ", max=" + std::to_string(maxAngle);
        isError_ = true;
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
