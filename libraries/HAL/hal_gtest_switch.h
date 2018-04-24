/**
 * hal_gtest_led.cpp
 * Hardware Abstraction for Google Test, simulating switch reads 
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
#ifndef HAL_SWITCH_H
#define HAL_SWITCH_H

// The ESP8266 is very short on GPIO. It is possible to read a switch on the 
// ADC input, by sampling and thresholding the value, thus gaining an extra 
// input. Uncomment 'ANALOG_MODE' to enable this.
// 1 is "normally high" and 0 is "normally low"
#define ANALOG_MODE 1
#define ANALOG_THRESH 512 // Set this to be the threshold analogue read value

#include <stdint.h>

typedef enum {
    SWITCH_TYPE_ANALOG = 0x01,
    SWITCH_TYPE_DIGITAL = 0x02
} SWITCH_TYPE;

class hal_Switch
{
    public:
    // Constructor
    hal_Switch(SWITCH_TYPE inType = SWITCH_TYPE_DIGITAL);

    // Get the state of the pins
    bool getPin();


    /// For testing: Set switch to "low" state
    void setLow();
    /// For testing: Set switch to "high" state
    void setHigh();
    /// For testing: Set internal mock ADC value
    void setAnalog(uint32_t inValue);
    /// For testing: Check if "getPin()" has been called
    bool isGetPinCalled();


    private:
    SWITCH_TYPE _type;
    uint32_t _value = 0;
    bool _config_done = false;
    bool _get_pin_called = false;

    void setup();
};

#endif