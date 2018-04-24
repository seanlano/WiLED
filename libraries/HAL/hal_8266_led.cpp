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

#include "hal_8266_led.h"

hal_LED::hal_LED(uint8_t inPin)
{
    _led_pin = inPin;
}

uint16_t hal_LED::getMaxPWM()
{
    return 1024;
}

void hal_LED::setPWM(uint16_t inValue)
{
    if(!_config_done)
    {
        setup();   
    }
    analogWrite(_led_pin, inValue);
}

void hal_LED::setup()
{
    _config_done = true;
    pinMode(_led_pin, OUTPUT);
}
