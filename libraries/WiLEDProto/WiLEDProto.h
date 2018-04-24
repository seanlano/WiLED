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

#ifdef GTEST_BUILD // i.e. if building the unit tests
  #include <stdlib.h>
  #include <stdint.h>
  #include <stdio.h>
  #include <string.h>
#else // i.e. a normal build
  #include <Arduino.h>
#endif
#include <CRC16.h>

#define MAXIMUM_STORED_ADDRESSES 100
#define MAXIMUM_MESSAGE_LENGTH 25
#define MAXIMUM_PAYLOAD_LENGTH 8

#define WiLP_Beacon 0x01
#define WiLP_Device_Status 0x02
#define WiLP_Set_Individual 0x10
#define WiLP_Set_Two_Individuals 0x11
#define WiLP_Set_Three_Individuals 0x12

#define WiLP_RETURN_SUCCESS 0
#define WiLP_RETURN_ADDED_ADDRESS 100
#define WiLP_RETURN_INVALID_MSG_CTR 200
#define WiLP_RETURN_INVALID_RST_CTR 201
#define WiLP_RETURN_INVALID_CHECKSUM 202
#define WiLP_RETURN_AT_MAX_ADDRESSES 203
#define WiLP_RETURN_INVALID_BUFFER 254
#define WiLP_RETURN_OTHER_ERROR 255
#define WiLP_RETURN_NOT_THIS_DEST 1
#define WiLP_RETURN_UNKNOWN_TYPE 2
#define WiLP_RETURN_NOT_INIT 3

// Arrange the storage locations of the arrays.
// _address_array is stored at location 0
#define STORAGE_ADDRESSES_LOCATION (0)
// _reset_counter_array is stored immediately following _address_array
#define STORAGE_RESET_LOCATION (STORAGE_ADDRESSES_LOCATION + sizeof(_address_array))
// _count_addresses is then stored after _reset_counter_array
#define STORAGE_COUNT_LOCATION (STORAGE_RESET_LOCATION + sizeof(_reset_counter_array))
// _self_reset_counter is then stored after _count_addresses
#define STORAGE_SELF_RESET_LOCATION (STORAGE_COUNT_LOCATION + sizeof(_count_addresses))


// Define a struct for storing and passing WiLED device information
struct WiLEDStatus {
  uint16_t address = 0;
  uint8_t level = 0;
  uint8_t attachedGroup[4] = {0};
};

// Define the class itself
class WiLEDProto {
  public:
    // Initialise with a single argument
    WiLEDProto(
      uint16_t inAddress,
      uint8_t (*inStorageReadCB)(uint16_t),
      void (*inStorageWriteCB)(uint16_t, uint8_t),
      void (*inStorageCommitCB)(void));

    void initStorage();

    // Set the 'Beacon' callback, which requires an argument of:
    //   - uint16_t source address
    //   - uint32_t beacon value (usually uptime)
    void setCallbackBeacon(void (*inBeaconRecvCB)(uint16_t, uint32_t));
    // Set the 'Set Individual' callback, which requires an argument of:
    //   - struct WiLEDStatus
    void setCallbackSetIndividual(void (*inBeaconRecvCB)(WiLEDStatus));
    // NOTE: setCallbackSetIndividual will be used for 'Two Individuals' and
    // 'Three Individuals' message types as well

    uint8_t processMessage(uint8_t* inBuffer);
    void handleLastMessage();

    uint8_t sendMessageBeacon(uint32_t inUptime);
    uint8_t sendMessageDeviceStatus(uint8_t inOutput, uint8_t inGroup1, uint8_t inGroup2, uint8_t inGroup3, uint8_t inGroup4);
    uint8_t sendMessageSetIndividual(uint8_t inOutput, uint16_t inAddress);
    uint8_t sendMessageSetTwoIndividuals(uint8_t inOutput, uint16_t inAddress1, uint16_t inAddress2);
    uint8_t sendMessageSetThreeIndividuals(uint8_t inOutput, uint16_t inAddress1, uint16_t inAddress2, uint16_t inAddress3);

    void copyToBuffer(uint8_t * inBuffer);

    uint8_t getLastReceivedType();
    uint16_t getLastReceivedSource();
    uint16_t getLastReceivedDestination();
    uint16_t getLastReceivedResetCounter();
    uint16_t getLastReceivedMessageCounter();
    uint8_t getLastReceivedMessageCounterValidation();

  protected:
    uint16_t _address = 0;
    uint16_t _self_reset_counter = 0;
    uint16_t _self_message_counter = 0;

    uint8_t _last_was_valid = false;
    uint8_t _last_received_type = 0x00;
    uint16_t _last_received_source = 0;
    uint16_t _last_received_destination = 0;
    uint16_t _last_received_reset_counter = 0;
    uint16_t _last_received_message_counter = 0;
    uint8_t _last_received_message_counter_validation = 0;
    uint8_t _last_received_payload_length = 0;
    uint8_t _last_received_payload[MAXIMUM_PAYLOAD_LENGTH] = {0};

    // Declare a "void-returning member-function of WiLEDProto" to be a type
    typedef void (WiLEDProto::*WiLEDMemFn)(void);
    WiLEDMemFn _process_callback = NULL;


    void (*_handler_cb_beacon)(uint16_t, uint32_t) = NULL;
    void _handleTypeBeacon();

    // Use the 'Set Individual' callback for 'Set Two' and 'Set Three' as well
    void (*_handler_cb_set_output)(WiLEDStatus) = NULL;
    void _handleTypeSetIndividual();
    void _handleTypeSetTwoIndividuals();
    void _handleTypeSetThreeIndividuals();

    uint8_t _outgoing_message_buffer[MAXIMUM_MESSAGE_LENGTH] = {0};
    uint8_t _outgoing_message_length = 0;

    void _setTypeByte(uint8_t inType);
    void _setDestinationByte(uint16_t inDestination);
    void _setPayloadByte(uint8_t inPayloadOffset, uint8_t inPayloadValue);

    void _wipeLastReceived();

    uint8_t _checkAndUpdateMessageCounter(uint16_t inAddress, uint16_t inResetCounter, uint16_t inMessageCounter);

    // Store callback functions for storage read and write (usually EEPROM)
    void (*_storage_write_callback)(uint16_t, uint8_t) = NULL;
    uint8_t (*_storage_read_callback)(uint16_t) = NULL;
    void (*_storage_commit_callback)(void) = NULL;

    // Store a count of how many unique addresses we have seen
    uint16_t _count_addresses = 0;

    uint8_t _restoreFromStorage_uint16t(uint16_t* outArray, uint16_t inStorageOffset, uint16_t inLength);
    uint8_t _addToStorage_uint16t(uint16_t* inArray, uint16_t inStorageOffset, uint16_t inLength);

    // Store (linked) arrays to track the other nodes' states, initialise to zero
    uint16_t _address_array[MAXIMUM_STORED_ADDRESSES] = {0};
    uint16_t _reset_counter_array[MAXIMUM_STORED_ADDRESSES] = {0};
    uint16_t _message_counter_array[MAXIMUM_STORED_ADDRESSES] = {0};

    // Store a struct of stuff related to this handler's current state
    WiLEDStatus _self_status;
};

#endif
