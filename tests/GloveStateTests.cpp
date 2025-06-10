#include "mock/MockMotor.hpp"
#include "mock/MockServo.hpp"
#include "models/GloveState.hpp"
#include <gtest/gtest.h>
#include <memory>

using namespace FingerFlexAid;

TEST(GloveStateTest, ConstructionAndAddDevices)
{
    GloveState glove;
    auto m1 = std::make_shared<MockMotor>("m1");
    auto s1 = std::make_shared<MockServo>("s1");
    glove.addMotor(m1);
    glove.addServo(s1);
    EXPECT_FALSE(glove.hasError());
}

TEST(GloveStateTest, ErrorDetection)
{
    GloveState glove;
    auto m1 = std::make_shared<MockMotor>("m1");
    auto s1 = std::make_shared<MockServo>("s1");
    glove.addMotor(m1);
    glove.addServo(s1);
    m1->simulateError(true);
    EXPECT_TRUE(glove.hasError());
    m1->simulateError(false);
    s1->simulateError(true);
    EXPECT_TRUE(glove.hasError());
    s1->simulateError(false);
    EXPECT_FALSE(glove.hasError());
}
