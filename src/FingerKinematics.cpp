#include "FingerKinematics.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

FingerKinematics::FingerKinematics()
    : m_fingerLength(1.0), // default length (adjust as needed)
      m_angleOffset(0.0)
{
}

void FingerKinematics::setParameters(double fingerLength, double angleOffset) {
    m_fingerLength = fingerLength;
    m_angleOffset  = angleOffset;
}

void FingerKinematics::updateSensorData(const std::vector<FingerData>& data) {
    m_sensorData = data;
}

double FingerKinematics::calculateAngle(const FingerData& data) const {
    // Example kinematic computation:
    // Convert the tilt reading into a servo angle using arcsin,
    // then adjust with an offset. In a real-world scenario,
    // this would be replaced with a more accurate kinematics algorithm.
    double ratio = data.tilt / m_fingerLength;
    if (ratio > 1.0) ratio = 1.0;
    if (ratio < -1.0) ratio = -1.0;
    double angle = std::asin(ratio) * 180.0 / M_PI + m_angleOffset;
    return std::clamp(angle, 0.0, 180.0);
}

std::vector<double> FingerKinematics::computeServoAngles() const {
    std::vector<double> servoAngles;
    for (const auto& d : m_sensorData) {
        servoAngles.push_back(calculateAngle(d));
    }
    return servoAngles;
}
