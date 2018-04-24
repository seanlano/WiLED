/**
 * hal_gtest_led.h
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

#include "hal_gtest_switch.h"

hal_Switch::hal_Switch(SWITCH_TYPE inType)
{
    _type = inType;
}

bool hal_Switch::getPin()
{
    if(!_config_done)
    {
        setup();   
    }

    _get_pin_called = true;

    if(_type == SWITCH_TYPE_ANALOG)
    {
        // Compare analogue value to the threshold
        uint16_t val = _value;
        #if ANALOG_MODE == 1
            // i.e. trigger if value goes above threshold
            if(val > ANALOG_THRESH)
            {
                return true;
            } 
            else 
            {
                return false;
            }
        #else
            // i.e. trigger if value goes below threshold
            if(val < ANALOG_THRESH)
            {
            return true;
            } 
            else 
            {
                return false;
            }
        #endif
    }
    else 
    {
        if(_value > 0)
        {
            return true;
        }
        else 
        {
            return false;
        }
    }
}

void hal_Switch::setLow()
{
    _value = 0;
}

void hal_Switch::setHigh()
{
    _value = 1;
}

void hal_Switch::setAnalog(uint32_t inValue)
{
    _value = inValue;
}

bool hal_Switch::isGetPinCalled()
{
    return _get_pin_called;
}

void hal_Switch::setup()
{
    // There isn't any setup needed when testing 

    _config_done = true;
}
