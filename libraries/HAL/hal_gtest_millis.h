/**
 * hal_gtest_millis.h
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

// The idea is that only one #include will provide each HAL part 
#ifndef HAL_MILLIS_H
#define HAL_MILLIS_H

#include <stdint.h>

class hal_Millis
{
    public:
    /// Constructor
    hal_Millis();

    /// Get the current time (in milliseconds) 
    uint64_t millis();

    /// Set the current time
    void setMillis(uint64_t inTime);

    /// Increment the time by a given amount
    void incrementMillis(uint64_t inIncrement);

    private:
    uint64_t _millis;
};

#endif