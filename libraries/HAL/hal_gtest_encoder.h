/**
 * hal_gtest_encoder.h
 * Hardware Abstraction for Google Test, rotary encoder simulation
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

// The idea is that only one #include will provide each HAL part 
#ifndef HAL_ENCODER_H
#define HAL_ENCODER_H

#include <stdint.h>
// Arduino language uses HIGH and LOW, which is more natural when reading
// code than true and false
#define HIGH true
#define LOW false

class hal_Encoder
{
    public:
    // Constructor
    hal_Encoder();

    // Get the state of the pins
    bool getPinA();
    bool getPinB();


    /// For testing: Set PinA state
    void setPinA(bool inValue);
    /// For testing: Set PinB state
    void setPinB(bool inValue);
    /// For testing: Check that getPin has been called
    bool isGetPinCalled();

    private:
    bool _config_done = false;
    bool _pinA = LOW;
    bool _pinB = LOW;
    bool _get_pin_called = false;

    void setup();
};

#endif