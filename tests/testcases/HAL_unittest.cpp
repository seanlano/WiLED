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
#include <Switch.h>
#include <RunMode.h>


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

class SwitchLibTest : public testing::Test {
    protected:
    SwitchLibTest() :
        // Need to use HIGH biasing for tests, electrically switches are usually
        // pulled high, then trigger low - but unit testing is the opposite 
        sw(&hw_switch, &hw_millis, HIGH)
    {
        // Initialise test fixture
        hw_millis.setMillis(0);
        hw_switch.setLow();
    }

    hal_Millis hw_millis;
    hal_Switch hw_switch;
    Switch sw;

    // Set up the test fixture
    //virtual void SetUp() {}

    //virtual void TearDown() {}
};

class RunModeLibTest : public testing::Test {
    protected:
    RunModeLibTest() :
        runmode(&hw_LED, &hw_millis)
    {
        // Initialise test fixture
    }

    hal_Millis hw_millis;
    hal_LED hw_LED;

    RunMode runmode;

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

TEST_F(SwitchLibTest, BasicSyntax)
{
    EXPECT_EQ(sw.poll(), false) << "Switch should not trigger";
    // Set the switch to "on"
    hw_switch.setHigh();
    // Set time to 1
    hw_millis.setMillis(1);
    EXPECT_EQ(sw.poll(), false) << "Switch should be low, not enough time has elapsed";
    EXPECT_EQ(hw_switch.isGetPinCalled(), true) << "GetPin should have been called";
    // Advance the clock to 45 milliseconds
    hw_millis.setMillis(45);
    EXPECT_EQ(sw.poll(), false) << "Switch should still be low, not enough time has elapsed";
    // Advance the clock to 51 milliseconds, which should trigger now
    hw_millis.setMillis(51);
    EXPECT_EQ(sw.poll(), true) << "Switch should now be high";
}

TEST_F(RunModeLibTest, BasicSyntax)
{
    EXPECT_EQ(runmode.getModeNormal(), true) << "RunMode should be normal";
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
// BEGIN tests for Switch library
////////////////////////////////////////////////////////////////////////////////

TEST_F(SwitchLibTest, Debounce30msNoiseThenOn)
{
    EXPECT_EQ(sw.poll(), false) << "Switch should be low at start";

    // Loop over an on/off bounce, for 30 milliseconds
    uint8_t ctr;
    for(ctr = 0; ctr < 30; ctr++)
    {
        // Set "on" during even cycles
        if(ctr % 2)
        {
            hw_switch.setHigh();
        }
        else
        // Set "off" during odd cycles
        {
            hw_switch.setLow();
        }
        // Set the time
        hw_millis.setMillis(ctr);
        // Check the switch state
        ASSERT_EQ(sw.poll(), false) << "Switch should not trigger during bounce, time is at millis: " << int(ctr);
    }
    // Now set the state to "on" and be stable until 50 milliseconds
    hw_switch.setHigh();
    for(ctr = 30; ctr < 50; ctr++)
    {
        // Set the time
        hw_millis.setMillis(ctr);
        // Check the switch state
        ASSERT_EQ(sw.poll(), false) << "Switch should not trigger during bounce, time is at millis: " << int(ctr);
    }
    // Switch should trigger high in the very next millisecond
    ctr++;
    // Set the time
    hw_millis.setMillis(ctr);
    EXPECT_EQ(sw.poll(), true) << "Switch should have triggered, time is at millis: " << int(ctr);
    EXPECT_EQ(sw.pushed(), true) << "Switch should have triggered, time is at millis: " << int(ctr);
}

TEST_F(SwitchLibTest, Debounce30msNoiseThenOff)
{
    EXPECT_EQ(sw.poll(), false) << "Switch should be low at start";

    // Loop over an on/off bounce, for 30 milliseconds
    uint8_t ctr;
    for(ctr = 0; ctr < 30; ctr++)
    {
        // Set "on" during even cycles
        if(ctr % 2)
        {
            hw_switch.setHigh();
        }
        else
        // Set "off" during odd cycles
        {
            hw_switch.setLow();
        }
        // Set the time
        hw_millis.setMillis(ctr);
        // Check the switch state
        ASSERT_EQ(sw.poll(), false) << "Switch should not trigger during bounce, time is at millis: " << int(ctr);
    }
    // Now set the state to "off" and be stable until 100 milliseconds
    hw_switch.setLow();
    for(ctr = 30; ctr < 100; ctr++)
    {
        // Set the time
        hw_millis.setMillis(ctr);
        // Check the switch state
        ASSERT_EQ(sw.poll(), false) << "Switch should not trigger because state is stable off, time is at millis: " << int(ctr);
        ASSERT_EQ(sw.pushed(), false) << "Switch should not have triggered, time is at millis: " << int(ctr);
    }
}

TEST_F(SwitchLibTest, Debounce30msNoiseThenLongPress)
{
    EXPECT_EQ(sw.poll(), false) << "Switch should be low at start";

    // Loop over an on/off bounce, for 30 milliseconds
    uint16_t ctr;
    for(ctr = 0; ctr < 30; ctr++)
    {
        // Set "on" during even cycles
        if(ctr % 2)
        {
            hw_switch.setHigh();
        }
        else
        // Set "off" during odd cycles
        {
            hw_switch.setLow();
        }
        // Set the time
        hw_millis.setMillis(ctr);
        // Check the switch state
        ASSERT_EQ(sw.poll(), false) << "Switch should not trigger during bounce, time is at millis: " << int(ctr);
    }
    
    // Now set the state to "on" and be stable until 50 milliseconds
    hw_switch.setHigh();
    for(ctr = 30; ctr < 50; ctr++)
    {
        // Set the time
        hw_millis.setMillis(ctr);
        // Check the switch state
        ASSERT_EQ(sw.poll(), false) << "Switch should not trigger during bounce, time is at millis: " << int(ctr);
    }
    // Switch should trigger high in the very next millisecond
    ctr = 50;
    // Set the time
    hw_millis.setMillis(ctr);
    EXPECT_EQ(sw.poll(), true) << "Switch should have triggered, time is at millis: " << int(ctr);
    EXPECT_EQ(sw.pushed(), true) << "Switch should have triggered, time is at millis: " << int(ctr);

    // The following poll() should then be false again
    ctr = 51;
    // Set the time
    hw_millis.setMillis(ctr);
    EXPECT_EQ(sw.poll(), false) << "Switch should not re-trigger, time is at millis: " << int(ctr);
    EXPECT_EQ(sw.pushed(), false) << "Switch should not re-trigger, time is at millis: " << int(ctr);
    
    // Keep the switch on until 450 ms
    for(ctr = 52; ctr < 451; ctr++)
    {
        // Set the time
        hw_millis.setMillis(ctr);
        // Check the switch state
        ASSERT_EQ(sw.poll(), false) << "Switch should not trigger waiting for long press, time is at millis: " << int(ctr);
        ASSERT_EQ(sw.longPress(), false) << "Long press should not trigger yet, time is at millis: " << int(ctr);
    }
    // Long press should trigger high in the very next millisecond (triggers one after the timeout)
    ctr = 451;
    // Set the time
    hw_millis.setMillis(ctr);
    EXPECT_EQ(sw.poll(), false) << "Switch should not have triggered, time is at millis: " << int(ctr);
    EXPECT_EQ(sw.longPress(), true) << "Long press should have triggered, time is at millis: " << int(ctr);
    EXPECT_EQ(sw.longPressLatch(), true) << "Long press latch should be triggered, time is at millis: " << int(ctr);
    
    // Long press should not trigger in the following millisecond
    ctr = 452;
    // Set the time
    hw_millis.setMillis(ctr);
    EXPECT_EQ(sw.poll(), false) << "Switch should not have triggered, time is at millis: " << int(ctr);
    EXPECT_EQ(sw.longPress(), false) << "Long press should not have re-triggered, time is at millis: " << int(ctr);
    EXPECT_EQ(sw.longPressLatch(), true) << "Long press latch should still be triggered, time is at millis: " << int(ctr);
}

////////////////////////////////////////////////////////////////////////////////
// END tests for Switch library
////////////////////////////////////////////////////////////////////////////////
// BEGIN tests for RunMode library
////////////////////////////////////////////////////////////////////////////////

TEST_F(RunModeLibTest, SetExactValue)
{
    hw_millis.setMillis(0);
    EXPECT_EQ(hw_LED.isSetPWMCalled(), false);
    runmode.update();
    runmode.setExact(100);
    hw_millis.setMillis(1);
    runmode.update();
    EXPECT_EQ(hw_LED.isSetPWMCalled(), true);
    EXPECT_EQ(hw_LED.getPWM(), 100);
}

TEST_F(RunModeLibTest, BlinkMode0)
{
    hw_millis.setMillis(0);
    runmode.setNormal();
    runmode.update();
    ASSERT_EQ(hw_LED.getPWM(), 0) << "LED value should be 0 at start";

    // Do nothing for 1000 ms
    int ctr;
    for(ctr = 1; ctr <= 1000; ctr++)
    {
        hw_millis.setMillis(ctr);
        runmode.update();
        ASSERT_EQ(hw_LED.getPWM(), 0) << "LED value should be 0 in normal mode";
    }

    // Set to blink mode "0"
    runmode.setBlink(0);
    for(ctr = 1001; ctr < 1200; ctr++)
    {
        // LED should be low for first cycle = 200 ms
        hw_millis.setMillis(ctr);
        runmode.update();
        ASSERT_EQ(hw_LED.getPWM(), 2) << "LED value should be 2. Time: " << ctr;
    }
    for(ctr = 1200; ctr < 2200; ctr++)
    {
        // LED should then be high for 5 time steps = 1000 ms
        hw_millis.setMillis(ctr);
        runmode.update();
        ASSERT_EQ(hw_LED.getPWM(), 255) << "LED value should be 255. Time: " << ctr;
    }
    for(ctr = 2200; ctr < 2600; ctr++)
    {
        // LED should be low for next 2 time steps = 400 ms
        hw_millis.setMillis(ctr);
        runmode.update();
        ASSERT_EQ(hw_LED.getPWM(), 2) << "LED value should be 2. Time: " << ctr;
    }
    for(ctr = 2600; ctr < 2800; ctr++)
    {
        // LED should be high for the next cycle
        hw_millis.setMillis(ctr);
        runmode.update();
        ASSERT_EQ(hw_LED.getPWM(), 255) << "LED value should be 255. Time: " << ctr;
    }

}

TEST_F(RunModeLibTest, BlinkMode1)
{
    hw_millis.setMillis(0);
    runmode.setNormal();
    runmode.update();
    ASSERT_EQ(hw_LED.getPWM(), 0) << "LED value should be 0 at start";

    // Do nothing for 1000 ms
    int ctr;
    for(ctr = 1; ctr <= 1000; ctr++)
    {
        hw_millis.setMillis(ctr);
        runmode.update();
        ASSERT_EQ(hw_LED.getPWM(), 0) << "LED value should be 0 in normal mode";
    }

    /**
     *  Set to blink mode "1"
     * 
     *  Output should be high for one cycle out of five
     *   0   1   2   3   4  
     *      ___
     *  ___|   |___ ___ ___ 
     */
    runmode.setBlink(1);
    for(ctr = 1001; ctr < 1200; ctr++)
    {
        // LED should be low for first cycle = 200 ms
        hw_millis.setMillis(ctr);
        runmode.update();
        ASSERT_EQ(hw_LED.getPWM(), 2) << "LED value should be 2. Time: " << ctr;
    }
    for(ctr = 1200; ctr < 1400; ctr++)
    {
        // LED should then be high for 1 time step = 200 ms
        hw_millis.setMillis(ctr);
        runmode.update();
        ASSERT_EQ(hw_LED.getPWM(), 255) << "LED value should be 255. Time: " << ctr;
    }
    for(ctr = 1400; ctr < 2200; ctr++)
    {
        // LED should be low for next 4 time steps = 800 ms
        hw_millis.setMillis(ctr);
        runmode.update();
        ASSERT_EQ(hw_LED.getPWM(), 2) << "LED value should be 2. Time: " << ctr;
    }
    for(ctr = 2200; ctr < 2400; ctr++)
    {
        // LED should be high again for the next 1 cycle
        hw_millis.setMillis(ctr);
        runmode.update();
        ASSERT_EQ(hw_LED.getPWM(), 255) << "LED value should be 255. Time: " << ctr;
    }
    for(ctr = 2400; ctr < 3200; ctr++)
    {
        // LED should be low for next 4 time steps = 800 ms
        hw_millis.setMillis(ctr);
        runmode.update();
        ASSERT_EQ(hw_LED.getPWM(), 2) << "LED value should be 2. Time: " << ctr;
    }
}

TEST_F(RunModeLibTest, BlinkMode2)
{
    hw_millis.setMillis(0);
    runmode.setNormal();
    runmode.update();
    ASSERT_EQ(hw_LED.getPWM(), 0) << "LED value should be 0 at start";

    // Do nothing for 1000 ms
    int ctr;
    for(ctr = 1; ctr <= 1000; ctr++)
    {
        hw_millis.setMillis(ctr);
        runmode.update();
        ASSERT_EQ(hw_LED.getPWM(), 0) << "LED value should be 0 in normal mode";
    }

    /**
     *  Set to blink mode "2"
     * 
     *  Output should be high for two cycles out of seven
     *   0   1   2   3   4   5   6
     *      ___     ___
     *  ___|   |___|   |___ ___ ___ 
     */
    runmode.setBlink(2);
    for(ctr = 1001; ctr < 1200; ctr++)
    {
        // LED should be low for first time step = 200 ms
        hw_millis.setMillis(ctr);
        runmode.update();
        ASSERT_EQ(hw_LED.getPWM(), 2) << "LED value should be 2. Time: " << ctr;
    }
    for(ctr = 1200; ctr < 1400; ctr++)
    {
        // LED should then be high for 1 time step = 200 ms
        hw_millis.setMillis(ctr);
        runmode.update();
        ASSERT_EQ(hw_LED.getPWM(), 255) << "LED value should be 255. Time: " << ctr;
    }
    for(ctr = 1400; ctr < 1600; ctr++)
    {
        // LED should be low for next 1 time step = 200 ms
        hw_millis.setMillis(ctr);
        runmode.update();
        ASSERT_EQ(hw_LED.getPWM(), 2) << "LED value should be 2. Time: " << ctr;
    }
    for(ctr = 1600; ctr < 1800; ctr++)
    {
        // LED should be high again for the next 1 time step = 200 ms
        hw_millis.setMillis(ctr);
        runmode.update();
        ASSERT_EQ(hw_LED.getPWM(), 255) << "LED value should be 255. Time: " << ctr;
    }
    for(ctr = 1800; ctr < 2600; ctr++)
    {
        // LED should be low for next 4 time steps = 800 ms
        hw_millis.setMillis(ctr);
        runmode.update();
        ASSERT_EQ(hw_LED.getPWM(), 2) << "LED value should be 2. Time: " << ctr;
    }
    for(ctr = 2600; ctr < 2800; ctr++)
    {
        // LED should then be high for 1 time step = 200 ms
        hw_millis.setMillis(ctr);
        runmode.update();
        ASSERT_EQ(hw_LED.getPWM(), 255) << "LED value should be 255. Time: " << ctr;
    }
    for(ctr = 2800; ctr < 3000; ctr++)
    {
        // LED should be low for next 1 time step = 200 ms
        hw_millis.setMillis(ctr);
        runmode.update();
        ASSERT_EQ(hw_LED.getPWM(), 2) << "LED value should be 2. Time: " << ctr;
    }
}

TEST_F(RunModeLibTest, BlinkMode3)
{
    hw_millis.setMillis(0);
    runmode.setNormal();
    runmode.update();
    ASSERT_EQ(hw_LED.getPWM(), 0) << "LED value should be 0 at start";

    // Do nothing for 1000 ms
    int ctr;
    for(ctr = 1; ctr <= 1000; ctr++)
    {
        hw_millis.setMillis(ctr);
        runmode.update();
        ASSERT_EQ(hw_LED.getPWM(), 0) << "LED value should be 0 in normal mode";
    }

    /**
     *  Set to blink mode "3"
     * 
     *  Output should be high for three cycles out of nine
     *   0   1   2   3   4   5   6   7   8
     *      ___     ___     ___ 
     *  ___|   |___|   |___|   |___ ___ ___ 
     */
    runmode.setBlink(3);
    for(ctr = 1001; ctr < 1200; ctr++)
    {
        // LED should be low for first time step = 200 ms
        hw_millis.setMillis(ctr);
        runmode.update();
        ASSERT_EQ(hw_LED.getPWM(), 2) << "LED value should be 2. Time: " << ctr;
    }
    for(ctr = 1200; ctr < 1400; ctr++)
    {
        // LED should then be high for 1 time step = 200 ms
        hw_millis.setMillis(ctr);
        runmode.update();
        ASSERT_EQ(hw_LED.getPWM(), 255) << "LED value should be 255. Time: " << ctr;
    }
    for(ctr = 1400; ctr < 1600; ctr++)
    {
        // LED should be low for next 1 time step = 200 ms
        hw_millis.setMillis(ctr);
        runmode.update();
        ASSERT_EQ(hw_LED.getPWM(), 2) << "LED value should be 2. Time: " << ctr;
    }
    for(ctr = 1600; ctr < 1800; ctr++)
    {
        // LED should be high again for the next 1 time step = 200 ms
        hw_millis.setMillis(ctr);
        runmode.update();
        ASSERT_EQ(hw_LED.getPWM(), 255) << "LED value should be 255. Time: " << ctr;
    }
    for(ctr = 1800; ctr < 2000; ctr++)
    {
        // LED should be low for next 1 time step = 200 ms
        hw_millis.setMillis(ctr);
        runmode.update();
        ASSERT_EQ(hw_LED.getPWM(), 2) << "LED value should be 2. Time: " << ctr;
    }
    for(ctr = 2000; ctr < 2200; ctr++)
    {
        // LED should be high again for the next 1 time step = 200 ms
        hw_millis.setMillis(ctr);
        runmode.update();
        ASSERT_EQ(hw_LED.getPWM(), 255) << "LED value should be 255. Time: " << ctr;
    }
    for(ctr = 2200; ctr < 3000; ctr++)
    {
        // LED should be low for next 4 time steps = 800 ms
        hw_millis.setMillis(ctr);
        runmode.update();
        ASSERT_EQ(hw_LED.getPWM(), 2) << "LED value should be 2. Time: " << ctr;
    }
    for(ctr = 3000; ctr < 3200; ctr++)
    {
        // LED should then be high for 1 time step = 200 ms
        hw_millis.setMillis(ctr);
        runmode.update();
        ASSERT_EQ(hw_LED.getPWM(), 255) << "LED value should be 255. Time: " << ctr;
    }
    for(ctr = 3200; ctr < 3400; ctr++)
    {
        // LED should be low for next 1 time step = 200 ms
        hw_millis.setMillis(ctr);
        runmode.update();
        ASSERT_EQ(hw_LED.getPWM(), 2) << "LED value should be 2. Time: " << ctr;
    }
}

////////////////////////////////////////////////////////////////////////////////
// END tests for RunMode library
////////////////////////////////////////////////////////////////////////////////
// BEGIN tests for 
////////////////////////////////////////////////////////////////////////////////
