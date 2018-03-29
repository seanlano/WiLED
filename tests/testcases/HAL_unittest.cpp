/**
 * HAL unit test program
 * Part of the "WiLED" project, https://github.com/seanlano/WiLED
 * 
 * Copyright (C) 2018 Sean Lanigan.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "lib/HAL/hal.h"
#include "lib/HAL/hal_gtest_millis.h"
#include "lib/HAL/hal_gtest_switch.h"
#include "lib/HAL/hal_gtest_led.h"
#include "lib/HAL/hal_gtest_encoder.h"
#include "gtest/gtest.h"


/// Test fixture
class HALMillisTest : public testing::Test {
    protected:
    HALMillisTest() 
    {
        // Initialise test fixture
    }

    // Create instance of millis HAL object
    hal_millis millis;

    // Set up the test fixture
    virtual void SetUp() 
    {
        millis.setMillis(0);
    }

    //virtual void TearDown() {}
};

class HALSwitchTest : public testing::Test {
    protected:
    HALSwitchTest() :
        sw(SWITCH_TYPE_DIGITAL)
    {
        // Initialise test fixture
    }

    // Create instance of millis HAL object
    hal_Switch sw;

    // Set up the test fixture
    virtual void SetUp() 
    {
        sw.setLow();
    }

    //virtual void TearDown() {}
};

class HALLEDTest : public testing::Test {
    protected:
    HALLEDTest()
    {
        // Initialise test fixture
    }

    // Create instance of millis HAL object
    hal_LED led;

    // Set up the test fixture
    //virtual void SetUp() {}

    //virtual void TearDown() {}
};

class HALEncoderTest : public testing::Test {
    protected:
    HALEncoderTest()
    {
        // Initialise test fixture
    }

    // Create instance of millis HAL object
    hal_Encoder encoder;

    // Set up the test fixture
    //virtual void SetUp() {}

    //virtual void TearDown() {}
};

////////////////////////////////////////////////////////////////////////////////
// BEGIN tests for basic syntax
////////////////////////////////////////////////////////////////////////////////

/// Test some very basic syntax things, mostly just to check code can compile 
TEST_F(HALMillisTest, BasicSyntax)
{
    EXPECT_EQ(millis.millis(), 0);
    millis.setMillis(1000);
    EXPECT_EQ(millis.millis(), 1000);
    millis.incrementMillis(1000);
    EXPECT_EQ(millis.millis(), 2000);
}

TEST_F(HALSwitchTest, BasicSyntax)
{
    EXPECT_EQ(sw.isGetPinCalled(), false);
    EXPECT_EQ(sw.getPin(), false);
    sw.setHigh();
    EXPECT_EQ(sw.getPin(), true);
}

TEST_F(HALLEDTest, BasicSyntax)
{
    EXPECT_EQ(led.isSetPWMCalled(), false);
    EXPECT_EQ(led.getPWM(), 0);
    led.setPWM(1024);
    EXPECT_EQ(led.getPWM(), 1024);
    led.setPWM(127);
    EXPECT_EQ(led.getPWM(), 127);
    EXPECT_EQ(led.isSetPWMCalled(), true);
}

TEST_F(HALEncoderTest, BasicSyntax)
{
    EXPECT_EQ(encoder.isGetPinCalled(), false);
    EXPECT_EQ(encoder.getPinA(), LOW);
    EXPECT_EQ(encoder.getPinB(), LOW);
    encoder.setPinA(HIGH);
    encoder.setPinB(HIGH);
    EXPECT_EQ(encoder.isGetPinCalled(), true);
    EXPECT_EQ(encoder.getPinA(), HIGH);
    EXPECT_EQ(encoder.getPinB(), HIGH);
}

////////////////////////////////////////////////////////////////////////////////
// END tests for basic syntax
////////////////////////////////////////////////////////////////////////////////
// BEGIN tests for 
////////////////////////////////////////////////////////////////////////////////


