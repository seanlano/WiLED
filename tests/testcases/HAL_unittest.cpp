/**
 * HAL_unittest.cpp
 * HAL unit test program, using the Google Test framework
 * 
 * Part of the "WiLED" project, https://github.com/seanlano/WiLED
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

#ifndef GTEST_BUILD
    #define GTEST_BUILD
#endif
#include "gtest/gtest.h"
#include <stdio.h>

#include <hal.h>
// The below would be included with hal.h, but VSCode doesn't automatically
// figure that out so these get highlighting and completion working
#include <hal_gtest_millis.h>
#include <hal_gtest_switch.h>
#include <hal_gtest_led.h>
#include <hal_gtest_encoder.h>

#include <Encoder.h>


/// Test fixtures
class HALMillisTest : public testing::Test {
    protected:
    HALMillisTest() 
    {
        // Initialise test fixture
    }

    // Create instance of millis HAL object
    hal_Millis millis;

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

    hal_Encoder encoder;

    // Set up the test fixture
    //virtual void SetUp() {}

    //virtual void TearDown() {}
};

class EncoderLibTest : public testing::Test {
    protected:
    EncoderLibTest() :
        encoder(&hw_encoder)
    {
        // Initialise test fixture
        ret_val = DIR_NONE;
    }

    hal_Encoder hw_encoder;
    Encoder encoder;
    uint8_t ret_val;

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

TEST_F(EncoderLibTest, BasicSyntax)
{
    ret_val = encoder.process();
    EXPECT_EQ(hw_encoder.isGetPinCalled(), true);
    EXPECT_EQ(ret_val, DIR_NONE);
}

////////////////////////////////////////////////////////////////////////////////
// END tests for basic syntax
////////////////////////////////////////////////////////////////////////////////
// BEGIN tests for Encoder library
////////////////////////////////////////////////////////////////////////////////

TEST_F(EncoderLibTest, RotateCCW)
{
    // Check the initial state is DIR_NONE
    ret_val = encoder.process();
    EXPECT_EQ(hw_encoder.isGetPinCalled(), true);
    EXPECT_EQ(ret_val, DIR_NONE) << "Direction should be 'none' after initialisation";

    // Inputs are active high, so set to HIGH state
    hw_encoder.setPinA(HIGH);
    hw_encoder.setPinB(HIGH);
    ret_val = encoder.process();
    EXPECT_EQ(ret_val, DIR_NONE) << "Direction should be 'none' at 0 step";

    // 1/4 step
    hw_encoder.setPinA(LOW);
    hw_encoder.setPinB(HIGH);
    ret_val = encoder.process();
    EXPECT_EQ(ret_val, DIR_NONE) << "Direction should be 'none' at 1/4 step";
    // 1/2 step
    hw_encoder.setPinA(LOW);
    hw_encoder.setPinB(LOW);
    ret_val = encoder.process();
    EXPECT_EQ(ret_val, DIR_NONE)  << "Direction should be 'none' at 1/2 step"; // Half-step is disabled
    // 3/4 step
    hw_encoder.setPinA(HIGH);
    hw_encoder.setPinB(LOW);
    ret_val = encoder.process();
    EXPECT_EQ(ret_val, DIR_NONE)  << "Direction should be 'none' at 3/4 step";
    // Full step
    hw_encoder.setPinA(HIGH);
    hw_encoder.setPinB(HIGH);
    ret_val = encoder.process();
    EXPECT_EQ(ret_val, DIR_CCW)  << "Direction should be 'CW' at full step";
}

TEST_F(EncoderLibTest, RotateCW)
{
    // Check the initial state is DIR_NONE
    ret_val = encoder.process();
    EXPECT_EQ(hw_encoder.isGetPinCalled(), true);
    EXPECT_EQ(ret_val, DIR_NONE) << "Direction should be 'none' after initialisation";

    // Inputs are active high, so set to HIGH state
    hw_encoder.setPinA(HIGH);
    hw_encoder.setPinB(HIGH);
    ret_val = encoder.process();
    EXPECT_EQ(ret_val, DIR_NONE) << "Direction should be 'none' at 0 step";

    // 1/4 step
    hw_encoder.setPinA(HIGH);
    hw_encoder.setPinB(LOW);
    ret_val = encoder.process();
    EXPECT_EQ(ret_val, DIR_NONE) << "Direction should be 'none' at 1/4 step";
    // 1/2 step
    hw_encoder.setPinA(LOW);
    hw_encoder.setPinB(LOW);
    ret_val = encoder.process();
    EXPECT_EQ(ret_val, DIR_NONE)  << "Direction should be 'none' at 1/2 step"; // Half-step is disabled
    // 3/4 step
    hw_encoder.setPinA(LOW);
    hw_encoder.setPinB(HIGH);
    ret_val = encoder.process();
    EXPECT_EQ(ret_val, DIR_NONE)  << "Direction should be 'none' at 3/4 step";
    // Full step
    hw_encoder.setPinA(HIGH);
    hw_encoder.setPinB(HIGH);
    ret_val = encoder.process();
    EXPECT_EQ(ret_val, DIR_CW)  << "Direction should be 'CW' at full step";
}

TEST_F(EncoderLibTest, DebounceCW)
{
    // Check the initial state is DIR_NONE
    ret_val = encoder.process();
    EXPECT_EQ(hw_encoder.isGetPinCalled(), true);
    EXPECT_EQ(ret_val, DIR_NONE) << "Direction should be 'none' after initialisation";

    // Inputs are active high, so set to HIGH state
    hw_encoder.setPinA(HIGH);
    hw_encoder.setPinB(HIGH);
    ret_val = encoder.process();
    EXPECT_EQ(ret_val, DIR_NONE) << "Direction should be 'none' at 0 step";

    // 1/4 step, 01
    hw_encoder.setPinA(HIGH);
    hw_encoder.setPinB(LOW);
    ret_val = encoder.process();
    EXPECT_EQ(ret_val, DIR_NONE) << "Direction should be 'none' at 1/4 step";
    // Back to 00
    hw_encoder.setPinA(HIGH);
    hw_encoder.setPinB(HIGH);
    ret_val = encoder.process();
    EXPECT_EQ(ret_val, DIR_NONE)  << "Direction should be 'none'";
    // 1/4 step again, 01
    hw_encoder.setPinA(HIGH);
    hw_encoder.setPinB(LOW);
    ret_val = encoder.process();
    EXPECT_EQ(ret_val, DIR_NONE) << "Direction should be 'none' at 1/4 step";
    // 1/2 step, 11
    hw_encoder.setPinA(LOW);
    hw_encoder.setPinB(LOW);
    ret_val = encoder.process();
    EXPECT_EQ(ret_val, DIR_NONE)  << "Direction should be 'none' at 1/2 step";
    // Back to 1/4 step again, 01
    hw_encoder.setPinA(HIGH);
    hw_encoder.setPinB(LOW);
    ret_val = encoder.process();
    EXPECT_EQ(ret_val, DIR_NONE) << "Direction should be 'none' at 1/4 step";
    // 1/2 step again, 11
    hw_encoder.setPinA(LOW);
    hw_encoder.setPinB(LOW);
    ret_val = encoder.process();
    EXPECT_EQ(ret_val, DIR_NONE)  << "Direction should be 'none' at 1/2 step";
    // 3/4 step, 10
    hw_encoder.setPinA(LOW);
    hw_encoder.setPinB(HIGH);
    ret_val = encoder.process();
    EXPECT_EQ(ret_val, DIR_NONE)  << "Direction should be 'none' at 3/4 step";
    // 1/2 step again, 11
    hw_encoder.setPinA(LOW);
    hw_encoder.setPinB(LOW);
    ret_val = encoder.process();
    EXPECT_EQ(ret_val, DIR_NONE)  << "Direction should be 'none' at 1/2 step";
    // 3/4 step again, 10
    hw_encoder.setPinA(LOW);
    hw_encoder.setPinB(HIGH);
    ret_val = encoder.process();
    EXPECT_EQ(ret_val, DIR_NONE)  << "Direction should be 'none' at 3/4 step";
    // Full step, 00
    hw_encoder.setPinA(HIGH);
    hw_encoder.setPinB(HIGH);
    ret_val = encoder.process();
    EXPECT_EQ(ret_val, DIR_CW)  << "Direction should be 'CW' at full step";
}

////////////////////////////////////////////////////////////////////////////////
// END tests for Encoder library
////////////////////////////////////////////////////////////////////////////////