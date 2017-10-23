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

#include <Arduino.h>

#define MAXIMUM_STORED_ADDRESSES 255
#define MAXIMUM_MESSAGE_LENGTH 25
#define MAXIMUM_PAYLOAD_LENGTH 8

#define WiLP_Beacon 0x01
#define WiLP_Device_Status 0x02

#define WiLP_RETURN_SUCCESS 0
#define WiLP_RETURN_INVALID_BUFFER 255
#define WiLP_RETURN_NOT_THIS_DEST 1
#define WiLP_RETURN_UNKNOWN_TYPE 2

struct AddressTracker {
  uint16_t address;
  uint16_t messageCounter;
  uint16_t resetCounter;
};


class WiLEDProto {
  public:
    // Initialise with a single argument
    WiLEDProto(uint16_t inAddress);
    // Initialise with reset counter callbacks
    //WiLEDProto(
    //  uint16_t inAddress,
    //  uint8_t (*inSetResetCounterCallback)(uint16_t address, uint16_t reset_counter),
    //  uint16_t (*inReadResetCounterCallback)(uint16_t address)
    //);
    uint8_t processMessage(uint8_t* inBuffer);

    uint8_t sendMessageBeacon(uint32_t inUptime);
    uint8_t sendMessageDeviceStatus(uint8_t inOutput, uint8_t inGroup1, uint8_t inGroup2, uint8_t inGroup3, uint8_t inGroup4);

    void copyToBuffer(uint8_t * inBuffer);

    uint8_t getLastReceivedType();
    uint16_t getLastReceivedSource();
    uint16_t getLastReceivedDestination();
    uint16_t getLastReceivedResetCounter();
    uint16_t getLastReceivedMessageCounter();

  protected:
    uint16_t __address = 0;
    uint16_t __self_message_counter = 0;

    uint8_t __last_received_type = 0x00;
    uint16_t __last_received_source = 0;
    uint16_t __last_received_destination = 0;
    uint16_t __last_received_reset_counter = 0;
    uint16_t __last_received_message_counter = 0;
    uint8_t __last_received_payload_length = 0;
    uint8_t __last_received_payload[MAXIMUM_PAYLOAD_LENGTH];

    uint8_t __outgoing_message_buffer[MAXIMUM_MESSAGE_LENGTH];

    void __setTypeByte(uint8_t inType);
    void __setDestinationByte(uint16_t inDestination);
    void __setPayloadByte(uint8_t inPayloadOffset, uint8_t inPayloadValue);

    //uint8_t (*__cb_set_reset_counter)(uint16_t address, uint16_t reset_counter);
    //uint16_t (*__cb_read_reset_counter)(uint16_t address);

    AddressTracker __addresses[MAXIMUM_STORED_ADDRESSES];
};


#endif
