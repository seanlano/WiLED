/*
* hal_8266_led.h
* Hardware Abstraction for ESP8266, providing LED access 
* 
* Part of the "WiLED" project, https://github.com/seanlano/WiLED
* Copyright (C) 2017 Sean Lanigan.
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

#include "hal_8266_encoder.h"

hal_Encoder::hal_Encoder(uint8_t inPinA, uint8_t inPinB)
{
    _encoder_pin_A = inPinA;
    _encoder_pin_B = inPinB;
}

bool hal_Encoder::getPinA()
{
    if(!_config_done)
    {
        setup();   
    }
    return digitalRead(_encoder_pin_A);
}

bool hal_Encoder::getPinB()
{
    if(!_config_done)
    {
        setup();   
    }
    return digitalRead(_encoder_pin_B);
}

void hal_Encoder::setup()
{
    _config_done = true;
    // The ESP8266 has support for internal pull-ups, so use them
    pinMode(_encoder_pin_A, INPUT_PULLUP);
    pinMode(_encoder_pin_B, INPUT_PULLUP);
}
