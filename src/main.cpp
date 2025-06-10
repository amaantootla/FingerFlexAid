#include "mock/MockMotor.hpp"
#include "mock/MockServo.hpp"
#include "models/GloveState.hpp"
#include "models/Motor.hpp"
#include "models/Servo.hpp"
#include <atomic>
#include <chrono>
#include <iostream>
#include <memory>
#include <thread>

using namespace FingerFlexAid;

int main()
{
    std::cout << "FingerFlexAid - Interactive Demo (press Enter to exit)\n";

    GloveState glove;

    // Create real and mock devices
    auto realMotor = std::make_shared<Motor>("real_motor_1", 1000, 10);
    auto mockMotor = std::make_shared<MockMotor>("mock_motor_1");
    auto realServo = std::make_shared<ServoImpl>(0, 180, 50, "real_servo_1");
    auto mockServo = std::make_shared<MockServo>("mock_servo_1");

    glove.addMotor(realMotor);
    glove.addMotor(mockMotor);
    glove.addServo(realServo);
    glove.addServo(mockServo);

    std::atomic<bool> running{true};
    std::thread inputThread([&running]() {
        std::cin.get();
        running = false;
    });

    int tick = 0;
    while (running)
    {
        // Simulate some actions
        double speed = (tick % 2 == 0) ? 200 : -200;
        double angle = (tick % 2 == 0) ? 45 : 135;
        realMotor->setSpeed(speed);
        mockMotor->setSpeed(speed);
        realServo->setAngle(angle);
        mockServo->setAngle(angle);

        // Every 5 ticks, simulate an error on a mock device
        if (tick % 5 == 0 && tick != 0)
        {
            mockMotor->simulateError("Simulated motor error");
            mockServo->simulateError(true);
        }
        // Every 7 ticks, clear errors
        if (tick % 7 == 0 && tick != 0)
        {
            mockMotor->clearError();
            mockServo->simulateError(false);
        }

        glove.update();

        // Print state
        std::cout << "Tick: " << tick << "\n";
        std::cout << "  RealMotor speed: " << realMotor->getSpeed()
                  << ", error: " << (realMotor->isError() ? realMotor->getErrorMessage() : "none") << "\n";
        std::cout << "  MockMotor speed: " << mockMotor->getCurrentSpeed()
                  << ", error: " << (mockMotor->isError() ? *mockMotor->getLastError() : "none") << "\n";
        std::cout << "  RealServo angle: " << realServo->getAngle()
                  << ", error: " << (realServo->hasError() ? "yes" : "no") << "\n";
        std::cout << "  MockServo angle: " << mockServo->getCurrentAngle()
                  << ", error: " << (mockServo->isError() ? *mockServo->getLastError() : "none") << "\n";
        std::cout << "  GloveState error: " << (glove.hasError() ? "YES" : "no") << "\n";
        std::cout << "-----------------------------\n";

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        ++tick;
    }
    inputThread.join();
    std::cout << "Exiting demo.\n";
    return 0;
}
