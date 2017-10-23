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

#include "WiLEDProto.h"


// Initialise the WiLEDProto class with it's address
WiLEDProto::WiLEDProto(uint16_t inAddress){
  // Store the address, must not be 0 or 65535
  if(inAddress > 0 && inAddress < 65535){
    __address = inAddress;
  } else {
    __address = 0;
  }
  // Set magic number
  __outgoing_message_buffer[0] = 0xAA;
  // Set source address (as big-endian 2-byte number)
  __outgoing_message_buffer[1] = (__address >> 8);
  __outgoing_message_buffer[2] = (__address);
}


// Send a "Beacon" message
uint8_t WiLEDProto::sendMessageBeacon(uint32_t inUptime){
  __setTypeByte(WiLP_Beacon);
  __setDestinationByte(0xFFFF);

  // Use right-shift to break into four (big endian) 1-byte blocks
  __setPayloadByte(0, (inUptime >> 24));
  __setPayloadByte(1, (inUptime >> 16));
  __setPayloadByte(2, (inUptime >> 8));
  __setPayloadByte(3, (inUptime));

  return WiLP_RETURN_SUCCESS;
}


// Set the "type" byte in the output buffer
void WiLEDProto::__setTypeByte(uint8_t inType){
  __outgoing_message_buffer[9] = inType;
}


// Set the "destination" bytes in the output buffer
void WiLEDProto::__setDestinationByte(uint16_t inDestination){
  // Set destination address (as big-endian 2-byte number)
  __outgoing_message_buffer[3] = (inDestination >> 8);
  __outgoing_message_buffer[4] = (inDestination);
}


// Set the specified "payload" byte in the output buffer
void WiLEDProto::__setPayloadByte(uint8_t inPayloadOffset, uint8_t inPayloadValue){
  __outgoing_message_buffer[10+inPayloadOffset] = inPayloadValue;
}


void WiLEDProto::copyToBuffer(uint8_t * inBuffer){
  /// copyToBuffer can only be called once, after calling, the
  /// message contents must be set again
  // Set reset counter bytes
  /// TODO

  // Set message counter bytes
  __self_message_counter++;
  __outgoing_message_buffer[7] = (__self_message_counter >> 8);
  __outgoing_message_buffer[8] = (__self_message_counter);

  // Copy internal buffer to provided address
  memcpy(inBuffer, __outgoing_message_buffer, MAXIMUM_MESSAGE_LENGTH);
}
