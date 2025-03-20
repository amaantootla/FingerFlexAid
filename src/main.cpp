#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "hardware.h"
#include "FingerKinematics.h"
#include "ServoController.h"

FingerKinematics kinematics;
ServoController* servoController = nullptr;
unsigned long lastFetchTime = 0;

void setup() {
    Serial.begin(BAUD_RATE);
    
    // Initialize WiFi
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(MEDIUM_DELAY);
        Serial.println("Connecting...");
    }
    Serial.println("Connected to WiFi");

    // Initialize servo controller with pins from hardware.h
    std::vector<int> servoPins = {
        SERVO_PIN_1, SERVO_PIN_2, SERVO_PIN_3, SERVO_PIN_4, SERVO_PIN_5,
        SERVO_PIN_6, SERVO_PIN_7, SERVO_PIN_8, SERVO_PIN_9, SERVO_PIN_10
    };
    servoController = new ServoController(servoPins);
    
    // Initialize kinematics with default parameters
    kinematics.setParameters(30.0, 10.0);
}

void loop() {
    unsigned long currentTime = millis();
    
    if (currentTime - lastFetchTime > FETCH_INTERVAL) {
        // Read flex sensor data
        std::vector<FingerData> sensorData;
        sensorData.reserve(NUM_FINGERS);
        
        const int flexPins[NUM_FINGERS] = {
            FLEX_PIN_1, FLEX_PIN_2, FLEX_PIN_3, FLEX_PIN_4, FLEX_PIN_5
        };
        
        for (int pin : flexPins) {
            double flexValue = analogRead(pin);
            sensorData.push_back({flexValue});
        }
        
        // Update kinematics with new sensor data
        kinematics.updateSensorData(sensorData);
        
        // Compute and apply servo angles
        auto servoAngles = kinematics.computeServoAngles();
        servoController->updateAllServos(servoAngles);
        
        lastFetchTime = currentTime;
    }
    
    delay(SHORT_DELAY);
}
