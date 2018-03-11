/*
* hal.h
* Hardware Abstraction Layer top-level header 
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

#ifndef HAL_H
#define HAL_H

// TODO: Conditional logic here depending on architecture
#define ESP8266
#ifdef ESP8266
    #include "hal_8266_led.h"
    #include <Arduino.h>
#endif

#endif