#pragma once

#include <atomic>
#include <mutex>
#include <string>

namespace FingerFlexAid
{

class Motor
{
  public:
    Motor(const std::string &id, double maxSpeed, double maxTorque);
    ~Motor();

    void setSpeed(double speed);
    double getSpeed() const;

    void setPosition(double position);
    double getPosition() const;

    double getMaxSpeed() const;
    double getMaxTorque() const;

    std::string getId() const;

    bool isMoving() const;
    bool isError() const;
    std::string getErrorMessage() const;
    void clearError();

    // Simulate error for testing
    void simulateError(const std::string &msg);

  private:
    std::string id_;
    double maxSpeed_;
    double maxTorque_;
    std::atomic<double> speed_;
    std::atomic<double> position_;
    std::atomic<bool> moving_;
    std::atomic<bool> error_;
    std::string errorMsg_;
    mutable std::mutex mutex_;
};

} // namespace FingerFlexAid
