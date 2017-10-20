/*
* WiLEDProto class
* Part of the "WiLED" project, https://github.com/seanlano/WiLED
* A C++ class for creating and processing messages using the WiLED
* Protocol.
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


#ifndef WILEDPROTO_H
#define WILEDPROTO_H

#define MAXIMUM_STORED_ADDRESSES 255

struct AddressTracker {
  uint16_t address;
  uint16_t messageCounter;
  uint16_t resetCounter;
};

class WiLEDProto {
  public:
    WiLEDProto(
      uint16_t inAddress,
      uint8_t (*inSetResetCounterCallback)(uint16_t address, uint16_t reset_counter),
      uint16_t (*inReadResetCounterCallback)(uint16_t address)
    );

  protected:
    uint8_t (*__cb_set_reset_counter)(uint16_t address, uint16_t reset_counter);
    uint16_t (*__cb_read_reset_counter)(uint16_t address);
    AddressTracker __addresses[MAXIMUM_STORED_ADDRESSES];
};


#endif
