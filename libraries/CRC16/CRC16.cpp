/*
* CRC16 checksum function
* Part of the "WiLED" project, https://github.com/seanlano/WiLED
* A C++ function for generating CRC16 checksums, in the XModem style.
* Copyright (C) 2017 Sean Lanigan.
* Originally based on 'Crc16' by Vincenzo Mennella, released under the MIT
* license. See here for the original:
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

#include "CRC16.h"

uint16_t CRC16(uint8_t data[], uint8_t start, uint16_t length)
{
  uint16_t polynomial = 0x1021;
  uint16_t xorIn = 0x0000;
  uint16_t msbMask = 0x8000;

	unsigned int crc = xorIn;

	int j;
	uint8_t c;
	unsigned int bit;

	if (length == 0) return crc;

	for (int i = start; i < (start + length); i++)
	{
		c = data[i];
		j = 0x80;

		while (j > 0)
		{
			bit = (unsigned int)(crc & msbMask);
			crc <<= 1;

			if ((c & j) != 0)
			{
				bit = (unsigned int)(bit ^ msbMask);
			}

			if (bit != 0)
			{
				crc ^= polynomial;
			}

			j >>= 1;
		}
	}
	return crc;
}
