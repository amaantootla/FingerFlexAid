#include "../src/mock/MockMotor.hpp"
#include "../src/models/Motor.hpp"
#include <chrono>
#include <gtest/gtest.h>
#include <thread>

using namespace FingerFlexAid;
using namespace std::chrono_literals;

class MockMotorTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        motor = std::make_unique<MockMotor>("test_motor");
    }

    void TearDown() override
    {
        motor.reset();
    }

    std::unique_ptr<MockMotor> motor;
};

TEST_F(MockMotorTest, InitialState)
{
    EXPECT_EQ(motor->getCurrentSpeed(), 0);
    EXPECT_EQ(motor->getCurrentPosition(), 0);
    EXPECT_FALSE(motor->isMoving());
    EXPECT_FALSE(motor->isError());
    EXPECT_FALSE(motor->getLastError().has_value());
    EXPECT_EQ(motor->getMaxSpeed(), 1000);
    EXPECT_EQ(motor->getAcceleration(), 1000);
}

TEST_F(MockMotorTest, SetSpeed)
{
    EXPECT_TRUE(motor->setSpeed(500));
    EXPECT_TRUE(motor->isMoving());

    std::this_thread::sleep_for(100ms);
    int16_t speedAfterShortTime = motor->getCurrentSpeed();
    EXPECT_GT(speedAfterShortTime, 0);
    EXPECT_LT(speedAfterShortTime, 500);

    std::this_thread::sleep_for(5000ms);
    int16_t speedAfterLongTime = motor->getCurrentSpeed();
    EXPECT_NEAR(speedAfterLongTime, 500, 50);
}

TEST_F(MockMotorTest, SetSpeedLimits)
{
    EXPECT_TRUE(motor->setSpeed(1000));
    EXPECT_FALSE(motor->setSpeed(1001));
    EXPECT_TRUE(motor->isError());
    EXPECT_TRUE(motor->getLastError().has_value());
}

TEST_F(MockMotorTest, SetPosition)
{
    EXPECT_TRUE(motor->setPosition(1000));
    EXPECT_TRUE(motor->isMoving());

    std::this_thread::sleep_for(100ms);
    EXPECT_GT(motor->getCurrentPosition(), 0);
}

TEST_F(MockMotorTest, Stop)
{
    EXPECT_TRUE(motor->setSpeed(500));
    EXPECT_TRUE(motor->isMoving());

    EXPECT_TRUE(motor->stop());
    EXPECT_FALSE(motor->isMoving());

    std::this_thread::sleep_for(100ms);
    EXPECT_NEAR(motor->getCurrentSpeed(), 0, 50);
}

TEST_F(MockMotorTest, EmergencyStop)
{
    EXPECT_TRUE(motor->setSpeed(500));
    EXPECT_TRUE(motor->isMoving());

    EXPECT_TRUE(motor->emergencyStop());
    EXPECT_FALSE(motor->isMoving());
    EXPECT_TRUE(motor->isError());
    EXPECT_EQ(motor->getLastError().value(), "Emergency stop activated");
}

TEST_F(MockMotorTest, SetMaxSpeed)
{
    EXPECT_TRUE(motor->setMaxSpeed(2000));
    EXPECT_EQ(motor->getMaxSpeed(), 2000);

    EXPECT_FALSE(motor->setMaxSpeed(0));
    EXPECT_TRUE(motor->isError());
}

TEST_F(MockMotorTest, SetAcceleration)
{
    EXPECT_TRUE(motor->setAcceleration(500));
    EXPECT_EQ(motor->getAcceleration(), 500);

    EXPECT_FALSE(motor->setAcceleration(0));
    EXPECT_TRUE(motor->isError());
}

TEST_F(MockMotorTest, SimulateError)
{
    motor->simulateError("Test error");
    EXPECT_TRUE(motor->isError());
    EXPECT_EQ(motor->getLastError().value(), "Test error");

    motor->clearError();
    EXPECT_FALSE(motor->isError());
    EXPECT_FALSE(motor->getLastError().has_value());
}

TEST_F(MockMotorTest, SimulateHardwareDelay)
{
    motor->simulateHardwareDelay(50ms);
    EXPECT_TRUE(motor->setSpeed(500));

    // With longer delay, position should change less
    auto initialPos = motor->getCurrentPosition();
    std::this_thread::sleep_for(100ms);
    auto posAfterDelay = motor->getCurrentPosition();

    EXPECT_GT(posAfterDelay, initialPos);
}

TEST_F(MockMotorTest, SpeedAcceleration)
{
    motor->setAcceleration(500);
    EXPECT_TRUE(motor->setSpeed(1000));

    std::this_thread::sleep_for(50ms);
    auto speedAfterShortTime = motor->getCurrentSpeed();

    std::this_thread::sleep_for(100ms);
    auto speedAfterLongTime = motor->getCurrentSpeed();

    EXPECT_GT(speedAfterLongTime, speedAfterShortTime);
    EXPECT_LT(speedAfterLongTime, 1000);
}

TEST(MotorModelTest, ConstructionAndProperties)
{
    Motor m("motor1", 100.0, 2.5);
    EXPECT_EQ(m.getId(), "motor1");
    EXPECT_DOUBLE_EQ(m.getMaxSpeed(), 100.0);
    EXPECT_DOUBLE_EQ(m.getMaxTorque(), 2.5);
    EXPECT_DOUBLE_EQ(m.getSpeed(), 0.0);
    EXPECT_DOUBLE_EQ(m.getPosition(), 0.0);
    EXPECT_FALSE(m.isMoving());
    EXPECT_FALSE(m.isError());
}

TEST(MotorModelTest, SetSpeedAndMoving)
{
    Motor m("motor2", 50.0, 1.0);
    m.setSpeed(25.0);
    EXPECT_DOUBLE_EQ(m.getSpeed(), 25.0);
    EXPECT_TRUE(m.isMoving());
    m.setSpeed(0.0);
    EXPECT_DOUBLE_EQ(m.getSpeed(), 0.0);
    EXPECT_FALSE(m.isMoving());
    m.setSpeed(100.0); // Should clamp to maxSpeed
    EXPECT_DOUBLE_EQ(m.getSpeed(), 50.0);
    m.setSpeed(-100.0); // Should clamp to -maxSpeed
    EXPECT_DOUBLE_EQ(m.getSpeed(), -50.0);
}

TEST(MotorModelTest, SetPosition)
{
    Motor m("motor3", 80.0, 1.5);
    m.setPosition(123.45);
    EXPECT_DOUBLE_EQ(m.getPosition(), 123.45);
}

TEST(MotorModelTest, ErrorSimulationAndClearing)
{
    Motor m("motor4", 60.0, 2.0);
    EXPECT_FALSE(m.isError());
    m.simulateError("test error");
    EXPECT_TRUE(m.isError());
    EXPECT_EQ(m.getErrorMessage(), "test error");
    m.setSpeed(30.0); // Should have no effect
    EXPECT_DOUBLE_EQ(m.getSpeed(), 0.0);
    m.clearError();
    EXPECT_FALSE(m.isError());
    EXPECT_EQ(m.getErrorMessage(), "");
    m.setSpeed(10.0);
    EXPECT_DOUBLE_EQ(m.getSpeed(), 10.0);
}
