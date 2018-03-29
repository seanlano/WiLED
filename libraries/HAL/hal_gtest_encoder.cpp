/**
 * hal_gtest_encoder.cpp
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

#include "hal_gtest_encoder.h"

hal_Encoder::hal_Encoder()
{
    _pinA = LOW;
    _pinB = LOW;
    _get_pin_called = false;
}

bool hal_Encoder::getPinA()
{
    if(!_config_done)
    {
        setup();   
    }
    _get_pin_called = true;
    return _pinA;
}

bool hal_Encoder::getPinB()
{
    if(!_config_done)
    {
        setup();   
    }
    _get_pin_called = true;
    return _pinB;
}

bool isGetPinCalled();

void hal_Encoder::setPinA(bool inValue)
{
    _pinA = inValue;
}

void hal_Encoder::setPinB(bool inValue)
{
    _pinB = inValue;
}

bool hal_Encoder::isGetPinCalled()
{
    return _get_pin_called;
}

void hal_Encoder::setup()
{
    // There isn't any setup needed when testing

    _config_done = true;
}
