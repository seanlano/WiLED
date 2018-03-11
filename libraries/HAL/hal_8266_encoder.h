/*
* hal_8266_encoder.h
* Hardware Abstraction for ESP8266, providing rotary encoder access 
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
#ifndef HAL_ENCODER_H
#define HAL_ENCODER_H

// This ESP8266 HAL depends on the Arduino framework
#include <Arduino.h>

class hal_Encoder
{
    public:
    // Constructor
    hal_Encoder(uint8_t inPinA, uint8_t inPinB);

    // Get the state of the pins
    bool getPinA();
    bool getPinB();

    private:
    uint8_t _encoder_pin_A, _encoder_pin_B;
    bool _config_done = false;

    void setup();
};

#endif