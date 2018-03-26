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

#include "hal_8266_switch.h"

hal_Switch::hal_Switch(uint8_t inPin, SWITCH_TYPE inType)
{
    _switch_pin = inPin;
    _type = inType;
}

bool hal_Switch::getPin()
{
    if(!_config_done)
    {
        setup();   
    }

    if(_type == SWITCH_TYPE_ANALOG)
    {
        // Compare analogue value to the threshold
        uint16_t val = analogRead(_switch_pin);
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
        return digitalRead(_switch_pin);
    }
}

void hal_Switch::setup()
{
    // Set the pin-mode if using a digital input
    if(_type == SWITCH_TYPE_DIGITAL)
    {
        // The ESP8266 has support for internal pull-ups, so use them
        pinMode(_switch_pin, INPUT_PULLUP);
    }
    // The ADC on the ESP8266 is already ready to go, no init needed

    _config_done = true;
}
