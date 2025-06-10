#include "../src/core/DeviceManagerImpl.hpp"
#include "../src/mock/MockMotor.hpp"
#include "../src/mock/MockServo.hpp"
#include <gtest/gtest.h>
#include <memory>

using namespace FingerFlexAid;

class DeviceManagerImplTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        manager = std::make_unique<DeviceManagerImpl>();
    }
    std::unique_ptr<DeviceManagerImpl> manager;
};

TEST_F(DeviceManagerImplTest, RegisterAndGetMotor)
{
    auto motor = std::make_shared<MockMotor>("m1");
    EXPECT_TRUE(manager->registerMotor("m1", motor));
    EXPECT_EQ(manager->getMotor("m1"), motor);
    EXPECT_FALSE(manager->registerMotor("m1", motor)); // duplicate
    EXPECT_EQ(manager->getMotorCount(), 1u);
}

TEST_F(DeviceManagerImplTest, RegisterAndGetServo)
{
    auto servo = std::make_shared<MockServo>("s1");
    EXPECT_TRUE(manager->registerServo("s1", servo));
    EXPECT_EQ(manager->getServo("s1"), servo);
    EXPECT_FALSE(manager->registerServo("s1", servo)); // duplicate
    EXPECT_EQ(manager->getServoCount(), 1u);
}

TEST_F(DeviceManagerImplTest, UnregisterDevice)
{
    auto motor = std::make_shared<MockMotor>("m2");
    auto servo = std::make_shared<MockServo>("s2");
    manager->registerMotor("m2", motor);
    manager->registerServo("s2", servo);
    EXPECT_TRUE(manager->unregisterDevice("m2"));
    EXPECT_TRUE(manager->unregisterDevice("s2"));
    EXPECT_EQ(manager->getMotorCount(), 0u);
    EXPECT_EQ(manager->getServoCount(), 0u);
}

TEST_F(DeviceManagerImplTest, InitializeAndShutdown)
{
    EXPECT_FALSE(manager->isInitialized());
    EXPECT_TRUE(manager->initializeAll());
    EXPECT_TRUE(manager->isInitialized());
    EXPECT_TRUE(manager->shutdownAll());
    EXPECT_FALSE(manager->isInitialized());
}

TEST_F(DeviceManagerImplTest, EmergencyStopAll)
{
    auto motor = std::make_shared<MockMotor>("m3");
    auto servo = std::make_shared<MockServo>("s3");
    manager->registerMotor("m3", motor);
    manager->registerServo("s3", servo);
    EXPECT_TRUE(manager->emergencyStopAll());
    EXPECT_TRUE(motor->isError());
    EXPECT_TRUE(servo->isError());
}

TEST_F(DeviceManagerImplTest, IsAnyDeviceMovingAndError)
{
    auto motor = std::make_shared<MockMotor>("m4");
    auto servo = std::make_shared<MockServo>("s4");
    manager->registerMotor("m4", motor);
    manager->registerServo("s4", servo);
    motor->setSpeed(100);
    servo->setAngle(45);
    EXPECT_TRUE(manager->isAnyDeviceMoving());
    motor->simulateError("fail");
    EXPECT_TRUE(manager->isAnyDeviceInError());
    auto errors = manager->getDevicesInError();
    EXPECT_EQ(errors.size(), 1u);
    EXPECT_EQ(errors[0], "m4");
}
