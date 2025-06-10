#include "models/GloveState.hpp"
#include <algorithm>
#include <iostream>

namespace FingerFlexAid
{

GloveState::GloveState()
{
}

void GloveState::addMotor(std::shared_ptr<Motor> motor)
{
    std::lock_guard<std::mutex> lock(mtx);
    motors.push_back(motor);
}

void GloveState::addServo(std::shared_ptr<Servo> servo)
{
    std::lock_guard<std::mutex> lock(mtx);
    servos.push_back(servo);
}

void GloveState::update()
{
    std::lock_guard<std::mutex> lock(mtx);
    for (const auto &m : motors)
    {
        if (m)
        {
            // Poll motor (e.g. update its state if needed)
            // (In a real implementation, you might call a polling or update method on Motor.)
            if (m->isError())
            {
                std::cerr << "Motor (" << m->getId() << ") reports error: " << m->getErrorMessage() << std::endl;
            }
        }
    }
    for (const auto &s : servos)
    {
        if (s)
        {
            if (s->hasError())
            {
                std::cerr << "Servo reports error." << std::endl;
            }
        }
    }
}

void GloveState::reset()
{
    std::lock_guard<std::mutex> lock(mtx);
    for (const auto &m : motors)
    {
        if (m)
        {
            m->clearError();
            m->setSpeed(0.0); // (or call a reset method if available)
        }
    }
    for (const auto &s : servos)
    {
        if (s)
        {
            s->setAngle(0.0); // (or call a reset method if available)
        }
    }
}

bool GloveState::hasError() const
{
    std::lock_guard<std::mutex> lock(mtx);
    for (const auto &m : motors)
    {
        if (m && m->isError())
            return true;
    }
    for (const auto &s : servos)
    {
        if (s && s->hasError())
            return true;
    }
    return false;
}

} // namespace FingerFlexAid
