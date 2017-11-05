/*
* CRC16 checksum function
* Part of the "WiLED" project, https://github.com/seanlano/WiLED
* A C++ function for generating CRC16 checksums, in the XModem style.
* Copyright (C) 2017 Sean Lanigan.
* Originally based on 'Crc16' by 'Vincenzo', released under the MIT license. See:
* https://github.com/vinmenn/Crc16/blob/aed958375acfd6edfcddbfcb3a42a861c77bb697/Crc16.h
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


#ifndef CRC16_H
#define CRC16_H

#ifdef GTEST_BUILD
  #include <stdlib.h>
  #include <stdint.h>
#else
  #include <Arduino.h>
#endif

uint16_t CRC16(uint8_t data[], uint8_t start, uint16_t length);

#endif
