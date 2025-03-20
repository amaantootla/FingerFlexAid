#ifndef HARDWARE_H
#define HARDWARE_H

// Number of fingers
constexpr int NUM_FINGERS = 5;

// ESP32 GPIO pins for servos
constexpr int SERVO_PIN_1 = 13;  // GPIO13
constexpr int SERVO_PIN_2 = 12;  // GPIO12
constexpr int SERVO_PIN_3 = 14;  // GPIO14
constexpr int SERVO_PIN_4 = 27;  // GPIO27
constexpr int SERVO_PIN_5 = 26;  // GPIO26
constexpr int SERVO_PIN_6 = 25;  // GPIO25
constexpr int SERVO_PIN_7 = 33;  // GPIO33
constexpr int SERVO_PIN_8 = 32;  // GPIO32
constexpr int SERVO_PIN_9 = 15;  // GPIO15
constexpr int SERVO_PIN_10 = 4;  // GPIO4

// ESP32 ADC pins for flex sensors
constexpr int FLEX_PIN_1 = 36;  // VP/GPIO36
constexpr int FLEX_PIN_2 = 39;  // VN/GPIO39
constexpr int FLEX_PIN_3 = 34;  // GPIO34
constexpr int FLEX_PIN_4 = 35;  // GPIO35
constexpr int FLEX_PIN_5 = 32;  // GPIO32

// System configuration
constexpr double ANGLE_CHANGE_THRESHOLD = 5.0;
constexpr unsigned long FETCH_INTERVAL = 100;    // milliseconds
constexpr int SHORT_DELAY = 20;                  // milliseconds
constexpr int MEDIUM_DELAY = 500;                // milliseconds
constexpr int BAUD_RATE = 115200;

// WiFi credentials
const char* WIFI_SSID = "YourNetworkName";
const char* WIFI_PASSWORD = "YourPassword";

#endif
