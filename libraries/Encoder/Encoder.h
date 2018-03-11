/*
* Encoder.h
* Rotary encoder handler
*
* Part of the "WiLED" project, https://github.com/seanlano/WiLED
* A C++ class for reading changes from a rotary encoder. 
* Copyright (C) 2017 Sean Lanigan, and original work 
* Copyright (C) 2011 Ben Buxton. (Contact: bb@cactii.net)
* Use git version control to view changes made to the original. 
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
*
*/

#ifndef Encoder_h
#define Encoder_h

#include <hal.h>

// Values returned by 'process'
// No complete step yet.
#define DIR_NONE 0x0
// Clockwise step.
#define DIR_CW 0x10
// Counter-clockwise step.
#define DIR_CCW 0x20

class Encoder
{
    public:
    Encoder(hal_Encoder *inEncoderPins);
    uint8_t process();
  
    private:
    uint8_t state;
    hal_Encoder *encoderPins;
};

#endif
 
