#ifndef FINGERKINEMATICS_H
#define FINGERKINEMATICS_H

#include <vector>
#include <algorithm> // for std::clamp

struct FingerData {
    double tilt; // Tilt in degrees read from sensor
};

class FingerKinematics {
public:
    FingerKinematics();

    // Update sensor data for the fingers.
    void updateSensorData(const std::vector<FingerData>& data);

    // Compute target servo angles based on the current sensor values using kinematics.
    std::vector<double> computeServoAngles() const;

    // Set kinematic parameters (e.g., finger length and offset)
    void setParameters(double fingerLength, double angleOffset);

private:
    // Compute the servo angle for a single finger using a simple trigonometric model.
    double calculateAngle(const FingerData& data) const;

    std::vector<FingerData> m_sensorData;
    double m_fingerLength;  // e.g. effective finger length (in degrees or cm as needed)
    double m_angleOffset;   // Offset to adjust the computed angle
};

#endif // FINGERKINEMATICS_H
