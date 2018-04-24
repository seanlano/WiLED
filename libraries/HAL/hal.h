/**
 * hal.h
 * Hardware Abstraction Layer top-level header 
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

#ifndef HAL_H
#define HAL_H

#if defined( ESP8266 )
    #include "hal_8266_led.h"
    #include "hal_8266_encoder.h"
    #include "hal_8266_switch.h"
    #include "hal_8266_radio.h"
    #include <Arduino.h>
#elif defined ( GTEST_BUILD )
    #include "hal_gtest_led.h"
    #include "hal_gtest_encoder.h"
    #include "hal_gtest_switch.h"
    #include "hal_gtest_millis.h"
#endif

#endif