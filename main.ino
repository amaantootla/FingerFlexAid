#include <WiFi.h>
#include <ESP32Servo.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "hardware.h"

unsigned long lastFetchTime = 0;
Servo fngrServos[numFngrs][2];
double currentAngles[numFngrs] = {0}; 
double previousAngles[numFngrs] = {0};
double targetAngles[5] = {0};
double assistAngles[5] = {0};
bool assistActive[numFngrs] = {false};

const int srvConnects[numFngrs][2] = {
  {SERVO_PIN_1, SERVO_PIN_2},
  {SERVO_PIN_3, SERVO_PIN_4},
  {SERVO_PIN_5, SERVO_PIN_6},
  {SERVO_PIN_7, SERVO_PIN_8},
  {SERVO_PIN_9, SERVO_PIN_10}
};
const int flxConnects[numFngrs] = {
  FLEX_PIN_1,
  FLEX_PIN_2,
  FLEX_PIN_3,
  FLEX_PIN_4,
  FLEX_PIN_5
};

void fetchAngles() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin("http://127.0.0.1:8000/api/get-angles"); // LAN
    int httpResponseCode = http.GET();
    JsonDocument jsonDoc; // https://arduinojson.org/v7/api/jsondocument/ (yes it is dynamic)
    DeserializationError error; // https://arduinojson.org/v/api/misc/deserializationerror/

    if (httpResponseCode == 200) {
      String response = http.getString();
      error = deserializeJson(jsonDoc, response);

      if (!error) {
        for (int i = 0; i < NUM_FNGRS; i++) {
          targetAngles[i] = jsonDoc["targetAngle"][i];
          assistAngles[i] = jsonDoc["assistAngle"][i];
        }
      }
      else {
        Serial.println(error.c_str());
      }
    }
    else {
      Serial.println(httpResponseCode);
    }

    http.end(); 
  }
}

void initServos() {
  for (int i = 0; i < numFngrs; i++) {
    fngrServos[i][0].attach(srvConnects[i][0]);
    fngrServos[i][1].attach(srvConnects[i][1]);
    fngrServos[i][0].write(uprightAngle);
    fngrServos[i][1].write(uprightAngle);
  }
}

int calculateServoSpeed(double currentAngle, double targetAngle) {
  double angleDiff = targetAngle - currentAngle;
  if (angleDiff > 0) {
    return map(angleDiff, flatAngle, straightAngle, uprightAngle, straightAngle);
  } else if (angleDiff < 0) {
    return map(angleDiff, -(straightAngle), flatAngle, flatAngle, uprightAngle);
  } else {
    return uprightAngle;
  }
}

void moveServos(int fngrIndex, double currentAngle) {
  int speed = calculateServoSpeed(currentAngle, targetAngles[fngrIndex]);

  fngrServos[fngrIndex][0].write(speed);        
  fngrServos[fngrIndex][1].write(straightAngle - speed);
}

void updateServoPositions() {
  for (int i = 0; i < numFngrs; i++) {
    double flexSensorVal = analogRead(flxConnects[i]);
    currentAngles[i] = map(flexSensorVal, flatAngle, max_analog, flatAngle, straightAngle);

    if (abs(currentAngles[i] - previousAngles[i]) >= angleChangeThreshold) {
      assistActive[i] = true;
    }

    if (assistActive[i]) {
      moveServos(i, currentAngles[i]);

      if (currentAngles[i] >= assistAngles[i]) {
        assistActive[i] = false;
        fngrServos[i][0].write(uprightAngle);  
        fngrServos[i][1].write(uprightAngle);  
      }
    }
    previousAngles[i] = currentAngles[i];
  }
}

void connectWiFi() { // https://docs.arduino.cc/libraries/wifi/
  WiFi.begin(wifiSSID, wifiPassword);
  while (WiFi.status() != WL_CONNECTED) {
    delay(medium_delay);
    Serial.println("Connecting...");
  }
  Serial.println("Connected.");
}

void loop() { // https://docs.arduino.cc/language-reference/en/structure/sketch/loop/
  unsigned long currentTime = millis();

  if (currentTime - lastFetchTime > fetchInterval) {
    fetchAngles();
    lastFetchTime = currentTime;
  }

  updateServoPositions();
  delay(short_delay);
}

void setup() { // https://docs.arduino.cc/language-reference/en/structure/sketch/setup/
  Serial.begin(buad_rate);
  connectWiFi();
  initServos();
}


