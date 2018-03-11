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

// The idea is that only one #include will provide each HAL part 
#ifndef HAL_LED_H
#define HAL_LED_H

// This ESP8266 HAL depends on the Arduino framework
#include <Arduino.h>

class hal_LED 
{
    public:
    // Constructor
    hal_LED(uint8_t inPin);

    // Return the maximum possible PWM value for this hardware
    uint16_t getMaxPWM();

    // Set the LED to the given PWM value
    void setPWM(uint16_t inValue);

    private:
    uint8_t _led_pin;
    bool _config_done = false;

    void setup();
};

#endif