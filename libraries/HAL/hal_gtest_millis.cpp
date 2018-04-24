/**
 * hal_gtest_millis.cpp
 * Hardware Abstraction for Google Test, providing system timing 
 * functions
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

#include "hal_gtest_millis.h"

hal_Millis::hal_Millis()
{
    // Initialise to zero
    _millis = 0;
}

uint64_t hal_Millis::millis()
{
    return _millis;
}

void hal_Millis::setMillis(uint64_t inTime)
{
    _millis = inTime;
}

void hal_Millis::incrementMillis(uint64_t inIncrement)
{
    _millis += inIncrement;
}