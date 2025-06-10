#include "MockMotor.hpp"
#include <algorithm>
#include <cmath>

namespace FingerFlexAid
{

MockMotor::MockMotor(const std::string &id)
    : Motor(id, 100.0, 1.0), id_(id), updateThread_([this]() { updatePosition(); })
{
}

MockMotor::~MockMotor()
{
    shouldStop_ = true;
    if (updateThread_.joinable())
    {
        updateThread_.join();
    }
}

bool MockMotor::setSpeed(int16_t speed)
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
    // Do not set currentSpeed_ here; let updatePosition handle it gradually
    isMoving_ = (speed != 0);
    return true;
}

bool MockMotor::setPosition(int32_t position)
{
    if (isError_)
    {
        return false;
    }

    if (!validatePosition(position))
    {
        std::lock_guard<std::mutex> lock(errorMutex_);
        lastError_ = "Invalid position value: " + std::to_string(position);
        isError_ = true;
        return false;
    }

    targetPosition_ = position;
    currentPosition_ = position;
    isMoving_ = true;
    return true;
}

bool MockMotor::stop()
{
    if (isError_)
    {
        return false;
    }

    targetSpeed_ = 0;
    currentSpeed_ = 0;
    isMoving_ = false;
    return true;
}

bool MockMotor::emergencyStop()
{
    targetSpeed_ = 0;
    currentSpeed_ = 0;
    isMoving_ = false;
    isError_ = true;

    std::lock_guard<std::mutex> lock(errorMutex_);
    lastError_ = "Emergency stop activated";
    return true;
}

int16_t MockMotor::getCurrentSpeed() const
{
    return currentSpeed_;
}

int32_t MockMotor::getCurrentPosition() const
{
    return currentPosition_;
}

bool MockMotor::isMoving() const
{
    return isMoving_;
}

bool MockMotor::isError() const
{
    return isError_;
}

std::optional<std::string> MockMotor::getLastError() const
{
    std::lock_guard<std::mutex> lock(errorMutex_);
    return lastError_;
}

bool MockMotor::setMaxSpeed(int16_t maxSpeed)
{
    if (maxSpeed <= 0)
    {
        std::lock_guard<std::mutex> lock(errorMutex_);
        lastError_ = "Invalid max speed value: " + std::to_string(maxSpeed);
        isError_ = true;
        return false;
    }

    maxSpeed_ = maxSpeed;
    return true;
}

bool MockMotor::setAcceleration(uint16_t acceleration)
{
    if (acceleration == 0)
    {
        std::lock_guard<std::mutex> lock(errorMutex_);
        lastError_ = "Invalid acceleration value: " + std::to_string(acceleration);
        isError_ = true;
        return false;
    }

    acceleration_ = acceleration;
    return true;
}

int16_t MockMotor::getMaxSpeed() const
{
    return maxSpeed_;
}

uint16_t MockMotor::getAcceleration() const
{
    return acceleration_;
}

void MockMotor::simulateHardwareDelay(std::chrono::milliseconds delay)
{
    hardwareDelay_.store(delay);
}

void MockMotor::simulateError(const std::string &error)
{
    std::lock_guard<std::mutex> lock(errorMutex_);
    lastError_ = error;
    isError_ = true;
}

void MockMotor::clearError()
{
    std::lock_guard<std::mutex> lock(errorMutex_);
    lastError_.reset();
    isError_ = false;
}

void MockMotor::updatePosition()
{
    while (!shouldStop_)
    {
        if (!isError_ && isMoving_)
        {
            // Calculate acceleration step based on current acceleration setting
            // Use a smaller divisor for smoother acceleration
            int16_t maxStep = std::max<int16_t>(1, static_cast<int16_t>(acceleration_ / 200));
            // Further limit the step to a fixed maximum to ensure gradual acceleration
            constexpr int16_t kMaxSpeedStep = 25; // Balanced for realism and test speed
            maxStep = std::min(maxStep, kMaxSpeedStep);

            // Calculate speed difference and apply acceleration/deceleration
            int16_t speedDiff = targetSpeed_ - currentSpeed_;
            if (speedDiff != 0)
            {
                // Use a non-linear acceleration curve for more realistic behavior
                double accelerationFactor = std::min(1.0, std::abs(speedDiff) / static_cast<double>(maxSpeed_));
                int16_t step =
                    std::clamp<int16_t>(static_cast<int16_t>(maxStep * (0.5 + accelerationFactor * 0.5)), 1,
                                        std::min<int16_t>(static_cast<int16_t>(std::abs(speedDiff)), kMaxSpeedStep));

                if (speedDiff > 0)
                {
                    currentSpeed_ += step;
                }
                else
                {
                    currentSpeed_ -= step;
                }
            }
            // Update position based on current speed with non-linear scaling
            if (currentSpeed_ != 0)
            {
                double speedFactor = std::abs(currentSpeed_) / static_cast<double>(maxSpeed_);
                double positionStep = (currentSpeed_ / 10.0) * (0.7 + speedFactor * 0.3);
                currentPosition_ += static_cast<int32_t>(positionStep);
            }
        }
        // Increase hardware delay for more observable gradual acceleration
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
}

bool MockMotor::validateSpeed(int16_t speed) const
{
    return std::abs(speed) <= maxSpeed_;
}

bool MockMotor::validatePosition(int32_t position) const
{
    const int32_t maxPositionChange = 1000000;
    return std::abs(position - currentPosition_) <= maxPositionChange;
}

} // namespace FingerFlexAid
