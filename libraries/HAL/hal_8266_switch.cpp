/*
* hal_8266_led.h
* Hardware Abstraction for ESP8266, providing LED access 
* 
* Part of the "WiLED" project, https://github.com/seanlano/WiLED
* A C++ class for controlling a PWM dimmable LED on an Arduino.
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

#include "hal_8266_switch.h"

hal_Switch::hal_Switch(uint8_t inPin)
{
    _switch_pin = inPin;
}

bool hal_Switch::getPin()
{
    if(!_config_done)
    {
        setup();   
    }
    return digitalRead(_switch_pin);
}

void hal_Switch::setup()
{
    _config_done = true;
    // The ESP8266 has support for internal pull-ups, so use them
    pinMode(_switch_pin, INPUT_PULLUP);
}
