#pragma once

#include <atomic>
#include <mutex>
#include <string>

namespace FingerFlexAid
{

class Servo
{
  public:
    virtual ~Servo() = default;
    virtual void setAngle(double angle) = 0;
    virtual double getAngle() const = 0;
    virtual void setSpeed(double speed) = 0;
    virtual double getSpeed() const = 0;
    virtual bool isMoving() const = 0;
    virtual void simulateError(bool simulate) = 0;
    virtual bool hasError() const = 0;
};

class ServoImpl : public Servo
{
  public:
    ServoImpl(double minAngle, double maxAngle, double defaultSpeed, std::string const &name = "ServoImpl");
    ~ServoImpl() override = default;
    void setAngle(double angle) override;
    double getAngle() const override;
    void setSpeed(double speed) override;
    double getSpeed() const override;
    bool isMoving() const override;
    void simulateError(bool simulate) override;
    bool hasError() const override;

  private:
    mutable std::mutex mtx;
    double _minAngle, _maxAngle, _currentAngle, _currentSpeed;
    std::atomic<bool> _moving, _simulateError, _hasError;
    std::string _name;
};

} // namespace FingerFlexAid
