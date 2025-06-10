#include "../src/mock/MockServo.hpp"
#include "../src/models/Servo.hpp"
#include <chrono>
#include <gtest/gtest.h>
#include <thread>

using namespace FingerFlexAid;
using namespace std::chrono_literals;

class MockServoTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        servo = std::make_unique<MockServo>("test_servo");
    }

    void TearDown() override
    {
        servo.reset();
    }

    std::unique_ptr<MockServo> servo;
};

TEST_F(MockServoTest, InitialState)
{
    EXPECT_EQ(servo->getCurrentAngle(), 90);
    EXPECT_EQ(servo->getCurrentSpeed(), 50);
    EXPECT_FALSE(servo->isMoving());
    EXPECT_FALSE(servo->isError());
    EXPECT_FALSE(servo->getLastError().has_value());
    EXPECT_EQ(servo->getMaxSpeed(), 100);
    auto [minAngle, maxAngle] = servo->getAngleLimits();
    EXPECT_EQ(minAngle, 0);
    EXPECT_EQ(maxAngle, 180);
}

TEST_F(MockServoTest, SetAngle)
{
    EXPECT_TRUE(servo->setAngle(45));
    EXPECT_TRUE(servo->isMoving());

    std::this_thread::sleep_for(100ms);
    EXPECT_LT(servo->getCurrentAngle(), 90);
}

TEST_F(MockServoTest, SetAngleLimits)
{
    EXPECT_TRUE(servo->setAngleLimits(30, 150));
    auto [minAngle, maxAngle] = servo->getAngleLimits();
    EXPECT_EQ(minAngle, 30);
    EXPECT_EQ(maxAngle, 150);

    EXPECT_FALSE(servo->setAngleLimits(150, 30));
    EXPECT_TRUE(servo->isError());

    EXPECT_FALSE(servo->setAngleLimits(0, 181));
    EXPECT_TRUE(servo->isError());
}

TEST_F(MockServoTest, SetSpeed)
{
    EXPECT_TRUE(servo->setSpeed(75));
    EXPECT_EQ(servo->getCurrentSpeed(), 75);

    EXPECT_FALSE(servo->setSpeed(101));
    EXPECT_TRUE(servo->isError());
}

TEST_F(MockServoTest, Stop)
{
    EXPECT_TRUE(servo->setAngle(45));
    EXPECT_TRUE(servo->isMoving());

    EXPECT_TRUE(servo->stop());
    EXPECT_FALSE(servo->isMoving());

    auto angleAtStop = servo->getCurrentAngle();
    std::this_thread::sleep_for(100ms);
    EXPECT_EQ(servo->getCurrentAngle(), angleAtStop);
}

TEST_F(MockServoTest, EmergencyStop)
{
    EXPECT_TRUE(servo->setAngle(45));
    EXPECT_TRUE(servo->isMoving());

    EXPECT_TRUE(servo->emergencyStop());
    EXPECT_FALSE(servo->isMoving());
    EXPECT_TRUE(servo->isError());
    EXPECT_EQ(servo->getLastError().value(), "Emergency stop activated");
}

TEST_F(MockServoTest, SetMaxSpeed)
{
    EXPECT_TRUE(servo->setMaxSpeed(75));
    EXPECT_EQ(servo->getMaxSpeed(), 75);

    EXPECT_FALSE(servo->setMaxSpeed(0));
    EXPECT_TRUE(servo->isError());

    EXPECT_FALSE(servo->setMaxSpeed(101));
    EXPECT_TRUE(servo->isError());
}

TEST_F(MockServoTest, SimulateError)
{
    servo->simulateError("Test error");
    EXPECT_TRUE(servo->isError());
    EXPECT_EQ(servo->getLastError().value(), "Test error");

    servo->clearError();
    EXPECT_FALSE(servo->isError());
    EXPECT_FALSE(servo->getLastError().has_value());
}

TEST_F(MockServoTest, SimulateHardwareDelay)
{
    servo->simulateHardwareDelay(50ms);
    EXPECT_TRUE(servo->setAngle(45));

    auto initialAngle = servo->getCurrentAngle();
    std::this_thread::sleep_for(100ms);
    auto angleAfterDelay = servo->getCurrentAngle();

    EXPECT_NE(angleAfterDelay, initialAngle);
    EXPECT_GT(std::abs(angleAfterDelay - initialAngle), 0);
}

TEST_F(MockServoTest, SpeedAffectsMovement)
{
    servo->setSpeed(25);
    EXPECT_TRUE(servo->setAngle(45));
    auto initialAngle = servo->getCurrentAngle();
    std::this_thread::sleep_for(50ms);
    auto angleAfterLowSpeed = servo->getCurrentAngle();
    auto lowSpeedChange = std::abs(angleAfterLowSpeed - initialAngle);

    servo->setAngle(90);
    std::this_thread::sleep_for(100ms);
    servo->setSpeed(75);
    EXPECT_TRUE(servo->setAngle(45));
    initialAngle = servo->getCurrentAngle();
    std::this_thread::sleep_for(50ms);
    auto angleAfterHighSpeed = servo->getCurrentAngle();
    auto highSpeedChange = std::abs(angleAfterHighSpeed - initialAngle);

    EXPECT_GT(highSpeedChange, lowSpeedChange);
}

TEST(ServoModelTest, ConstructionAndProperties)
{
    ServoImpl s(-90.0, 90.0, 100.0, "servo1");
    EXPECT_DOUBLE_EQ(s.getAngle(), -90.0);
    EXPECT_DOUBLE_EQ(s.getSpeed(), 100.0);
    EXPECT_FALSE(s.isMoving());
    EXPECT_FALSE(s.hasError());
}

TEST(ServoModelTest, SetAngleAndClamp)
{
    ServoImpl s(-45.0, 45.0, 50.0, "servo2");
    s.setAngle(30.0);
    EXPECT_DOUBLE_EQ(s.getAngle(), 30.0);
    s.setAngle(100.0);
    EXPECT_DOUBLE_EQ(s.getAngle(), 45.0);
    s.setAngle(-100.0);
    EXPECT_DOUBLE_EQ(s.getAngle(), -45.0);
}

TEST(ServoModelTest, SetSpeedAndMoving)
{
    ServoImpl s(-180.0, 180.0, 80.0, "servo3");
    s.setSpeed(40.0);
    EXPECT_DOUBLE_EQ(s.getSpeed(), 40.0);
    EXPECT_TRUE(s.isMoving());
    s.setSpeed(0.0);
    EXPECT_DOUBLE_EQ(s.getSpeed(), 0.0);
    EXPECT_FALSE(s.isMoving());
    s.setSpeed(100.0);
    EXPECT_DOUBLE_EQ(s.getSpeed(), 100.0);
}

TEST(ServoModelTest, ErrorSimulationAndClearing)
{
    ServoImpl s(-60.0, 60.0, 60.0, "servo4");
    EXPECT_FALSE(s.hasError());
    s.simulateError(true);
    s.setAngle(30.0);
    EXPECT_TRUE(s.hasError());
    EXPECT_DOUBLE_EQ(s.getAngle(), -60.0);
    s.simulateError(false);
    EXPECT_FALSE(s.hasError());
    s.setAngle(10.0);
    EXPECT_DOUBLE_EQ(s.getAngle(), 10.0);
}
