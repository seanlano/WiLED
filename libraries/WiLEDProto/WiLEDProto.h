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

#define MAXIMUM_STORED_ADDRESSES 100
#define MAXIMUM_MESSAGE_LENGTH 25
#define MAXIMUM_PAYLOAD_LENGTH 8

#define WiLP_Beacon 0x01
#define WiLP_Device_Status 0x02

#define WiLP_RETURN_SUCCESS 0
#define WiLP_RETURN_INVALID_MSG_CTR 200
#define WiLP_RETURN_INVALID_RST_CTR 201
#define WiLP_RETURN_ADDED_ADDRESS 202
#define WiLP_RETURN_AT_MAX_ADDRESSES 203
#define WiLP_RETURN_INVALID_BUFFER 254
#define WiLP_RETURN_OTHER_ERROR 255
#define WiLP_RETURN_NOT_THIS_DEST 1
#define WiLP_RETURN_UNKNOWN_TYPE 2
#define WiLP_RETURN_NOT_INIT 3

// Arrange the storage locations of the arrays.
// __address_array is stored at location 0
#define STORAGE_ADDRESSES_LOCATION (0)
// __reset_counter_array is stored immediately following __address_array
#define STORAGE_RESET_LOCATION (STORAGE_ADDRESSES_LOCATION + sizeof(__address_array))
// __count_addresses is then stored after __reset_counter_array
#define STORAGE_COUNT_LOCATION (STORAGE_RESET_LOCATION + sizeof(__reset_counter_array))
// __self_reset_counter is then stored after __count_addresses
#define STORAGE_SELF_RESET_LOCATION (STORAGE_COUNT_LOCATION + sizeof(__count_addresses))

class WiLEDProto {
  public:
    // Initialise with a single argument
    WiLEDProto(
      uint16_t inAddress,
      uint8_t (*inStorageReadCB)(uint16_t),
      void (*inStorageWriteCB)(uint16_t, uint8_t),
      void (*inStorageCommitCB)(void));

    void initStorage();

    uint8_t processMessage(uint8_t* inBuffer);

    uint8_t sendMessageBeacon(uint32_t inUptime);
    uint8_t sendMessageDeviceStatus(uint8_t inOutput, uint8_t inGroup1, uint8_t inGroup2, uint8_t inGroup3, uint8_t inGroup4);

    void copyToBuffer(uint8_t * inBuffer);

    uint8_t getLastReceivedType();
    uint16_t getLastReceivedSource();
    uint16_t getLastReceivedDestination();
    uint16_t getLastReceivedResetCounter();
    uint16_t getLastReceivedMessageCounter();
    uint8_t getLastReceivedMessageCounterValidation();

  protected:
    uint16_t __address = 0;
    uint16_t __self_reset_counter = 0;
    uint16_t __self_message_counter = 0;

    uint8_t __last_received_type = 0x00;
    uint16_t __last_received_source = 0;
    uint16_t __last_received_destination = 0;
    uint16_t __last_received_reset_counter = 0;
    uint16_t __last_received_message_counter = 0;
    uint8_t __last_received_message_counter_validation = 0;
    uint8_t __last_received_payload_length = 0;
    uint8_t __last_received_payload[MAXIMUM_PAYLOAD_LENGTH];

    uint8_t __outgoing_message_buffer[MAXIMUM_MESSAGE_LENGTH];

    void __setTypeByte(uint8_t inType);
    void __setDestinationByte(uint16_t inDestination);
    void __setPayloadByte(uint8_t inPayloadOffset, uint8_t inPayloadValue);

    uint8_t __checkAndUpdateMessageCounter(uint16_t inAddress, uint16_t inResetCounter, uint16_t inMessageCounter);

    // Store callback functions for storage read and write (usually EEPROM)
    void (*__storage_write_callback)(uint16_t, uint8_t) = 0;
    uint8_t (*__storage_read_callback)(uint16_t) = 0;
    void (*__storage_commit_callback)(void) = 0;

    // Store a count of how many unique addresses we have seen
    uint16_t __count_addresses = 0;

    uint8_t __restoreFromStorage_uint16t(uint16_t* outArray, uint16_t inStorageOffset, uint16_t inLength);
    uint8_t __addToStorage_uint16t(uint16_t* inArray, uint16_t inStorageOffset, uint16_t inLength);

    // Store (linked) arrays to track the other nodes' states, initialise to zero
    uint16_t __address_array[MAXIMUM_STORED_ADDRESSES] = {0};
    uint16_t __reset_counter_array[MAXIMUM_STORED_ADDRESSES] = {0};
    uint16_t __message_counter_array[MAXIMUM_STORED_ADDRESSES] = {0};
};


#endif
