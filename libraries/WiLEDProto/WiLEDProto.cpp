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

/// Initialise the WiLEDProto class with its address, and storage callbacks
WiLEDProto::WiLEDProto(
  uint16_t inAddress,
  uint8_t (*inStorageReadCB)(uint16_t),
  void (*inStorageWriteCB)(uint16_t, uint8_t),
  void (*inStorageCommitCB)(void))
{
  // Store callbacks
  __storage_read_callback = inStorageReadCB;
  __storage_write_callback = inStorageWriteCB;
  __storage_commit_callback = inStorageCommitCB;
  // Store the address, must not be 0 or 65535
  if(inAddress > 0 && inAddress < 65535){
    __self_status.address = inAddress;
  } else {
    __self_status.address = 0;
  }
  // Set magic number
  __outgoing_message_buffer[0] = 0xAA;
  // Set source address (as big-endian 2-byte number)
  __outgoing_message_buffer[1] = (__self_status.address >> 8);
  __outgoing_message_buffer[2] = (__self_status.address);
}

/// initStorage() should be called after setting up the storage device in the
/// main program, i.e. once storage callbacks are ready to be used.
void WiLEDProto::initStorage(){

  if(__storage_commit_callback != NULL && __storage_write_callback != NULL){
    // TODO: Check the return value of these?
    // Read the addresses and reset counter arrays from storage
    __restoreFromStorage_uint16t(__address_array, STORAGE_ADDRESSES_LOCATION, sizeof(__address_array));
    __restoreFromStorage_uint16t(__reset_counter_array, STORAGE_RESET_LOCATION, sizeof(__reset_counter_array));
    __restoreFromStorage_uint16t(&__count_addresses, STORAGE_COUNT_LOCATION, sizeof(__count_addresses));

    // Read this devices own reset counter, then increment it and store it
    __restoreFromStorage_uint16t(&__self_reset_counter, STORAGE_SELF_RESET_LOCATION, sizeof(__self_reset_counter));
    __self_reset_counter++;
    __addToStorage_uint16t(&__self_reset_counter, STORAGE_SELF_RESET_LOCATION, sizeof(__self_reset_counter));
    __storage_commit_callback();

    // Arduino-specific debug
    #ifdef ARDUINO_DEBUG
      Serial.print("Loaded addresses: ");
      Serial.println(__count_addresses);
      Serial.print("Addresses: ");
      for(uint16_t idx=0; idx < __count_addresses; idx++){
        Serial.print(__address_array[idx], HEX);
        Serial.print(", ");
      }
      Serial.println();
      Serial.print("This device's reset counter: ");
      Serial.println(__self_reset_counter);
      Serial.println();
    #endif
  }
}


// Process a received message
uint8_t WiLEDProto::processMessage(uint8_t* inBuffer){
  __last_was_valid = false;
  // Check if first byte is magic number
  if(inBuffer[0] != 0xAA){
    __wipeLastReceived();
    return WiLP_RETURN_INVALID_BUFFER;
  }
  // Store the received message destination (left shift)
  __last_received_destination = (inBuffer[3] << 8);
  __last_received_destination += inBuffer[4];
  // Store the received message source (left shift)
  __last_received_source = (inBuffer[1] << 8);
  __last_received_source += inBuffer[2];
  // Store the received message type byte
  __last_received_type = inBuffer[9];
  // Store the received reset counter (left shift)
  __last_received_reset_counter = (inBuffer[5] << 8);
  __last_received_reset_counter += inBuffer[6];
  // Store the received message counter
  __last_received_message_counter = (inBuffer[7] << 8);
  __last_received_message_counter += inBuffer[8];
  // Check if we are the destination
  if((__last_received_destination != __self_status.address) &&
     (__last_received_destination != 0xFFFF)){
    return WiLP_RETURN_NOT_THIS_DEST;
  }

  __last_received_message_counter_validation =
  __checkAndUpdateMessageCounter(__last_received_source,
                                 __last_received_reset_counter,
                                 __last_received_message_counter);

  // Determine the payload length and which process callback to use
  switch(__last_received_type){
    case WiLP_Beacon:
      __last_received_payload_length = 4;
      __process_callback = &WiLEDProto::__handleTypeBeacon;
      break;
    case WiLP_Set_Individual:
      __last_received_payload_length = 3;
      __process_callback = &WiLEDProto::__handleTypeSetIndividual;
      break;
    case WiLP_Set_Two_Individuals:
      __last_received_payload_length = 5;
      __process_callback = &WiLEDProto::__handleTypeSetTwoIndividuals;
      break;
    case WiLP_Set_Three_Individuals:
      __last_received_payload_length = 7;
      __process_callback = &WiLEDProto::__handleTypeSetThreeIndividuals;
      break;
    default:
      __last_received_payload_length = 0;
      __process_callback = NULL;
      return WiLP_RETURN_UNKNOWN_TYPE;
  }
  // Store the payload bytes in an array
  if(__last_received_payload_length > 0){
    memcpy(__last_received_payload, &inBuffer[10], __last_received_payload_length);
  }

  // Calculate the CRC16 checksum of the received message
  uint16_t checksum = CRC16(inBuffer, 0, 10+__last_received_payload_length);
  // Pull out the checksum in the received message
  uint16_t received_checksum;
  received_checksum  = (inBuffer[10+__last_received_payload_length] << 8);
  received_checksum += inBuffer[10+__last_received_payload_length+1];
  // Check the calculated checksum matches the received checksum
  if(checksum != received_checksum){
    return WiLP_RETURN_INVALID_CHECKSUM;
  }

  // If received message does not pass the validation, return with an error code
  if((__last_received_message_counter_validation == WiLP_RETURN_SUCCESS) ||
     (__last_received_message_counter_validation == WiLP_RETURN_ADDED_ADDRESS)){
    // Store the fact that this is a valid message
    __last_was_valid = true;
    // Return to caller with success
    return WiLP_RETURN_SUCCESS;
  } else {
    return __last_received_message_counter_validation;
  }
}


// Handle the callback for the last received message, if there is one
void WiLEDProto::handleLastMessage(){
  if((__process_callback != NULL) && (__last_was_valid == true)){
    // Run the callback, with funky C++ syntax for calling a member-function pointer
    (this->*__process_callback)();
  }
}


void WiLEDProto::copyToBuffer(uint8_t * inBuffer){
  /// copyToBuffer can only be called once. After calling, the
  /// message contents must be set again.

  // Increment and set message counter bytes (big endian)
  // If at maximum for uint16_t, increment reset counter and go back to zero
  if(__self_message_counter == 65535){
    __self_message_counter = 0;
    // Increment the reset counter and store it
    __self_reset_counter++;
    __addToStorage_uint16t(&__self_reset_counter,
                           STORAGE_SELF_RESET_LOCATION,
                           sizeof(__self_reset_counter));
    __storage_commit_callback();
  }
  __self_message_counter++;
  __outgoing_message_buffer[7] = (__self_message_counter >> 8);
  __outgoing_message_buffer[8] = (__self_message_counter);

  // Set reset counter bytes
  __outgoing_message_buffer[5] = (__self_reset_counter >> 8);
  __outgoing_message_buffer[6] = (__self_reset_counter);

  // Calculate and insert CRC16 checksum
  uint16_t checksum = CRC16(__outgoing_message_buffer,
                            0,
                            __outgoing_message_length-2);
  // Store the checksum in the last 2 bytes of the message
  __outgoing_message_buffer[__outgoing_message_length-2] = (checksum >> 8);
  __outgoing_message_buffer[__outgoing_message_length-1] = (checksum);

  // Copy internal buffer to provided address
  memcpy(inBuffer, __outgoing_message_buffer, MAXIMUM_MESSAGE_LENGTH);

  // Wipe message buffer (keep magic number and source address)
  for(uint8_t idx = 3; idx<MAXIMUM_MESSAGE_LENGTH; idx++){
    __outgoing_message_buffer[idx] = 0x00;
  }
  __outgoing_message_length = 0;
}


////////////////////////////////////////////////////////////////////////////////
// BEGIN 'sendMessageTYPE' section
//
// Each WiLP message type has a 'sendMessageTYPE' function. These functions
// format the internal payload buffer with their specific data, possibly taking
// one or more arguments to do so.
// They must return to the calling program a uint8_t return code, of the values
// set through #define statements in WiLEDProto.h, eg. WiLP_RETURN_SUCCESS etc.
//
/// Send a "Beacon" message
uint8_t WiLEDProto::sendMessageBeacon(uint32_t inUptime){
  __setTypeByte(WiLP_Beacon);
  __setDestinationByte(0xFFFF);

  // Use right-shift to break into four (big endian) 1-byte blocks
  __setPayloadByte(0, (inUptime >> 24));
  __setPayloadByte(1, (inUptime >> 16));
  __setPayloadByte(2, (inUptime >> 8));
  __setPayloadByte(3, (inUptime));

  // Store the message length. 10 bytes header, 4 bytes payload, 2 bytes checksum
  __outgoing_message_length = 16;

  return WiLP_RETURN_SUCCESS;
}
//
/// Send a 'Set Individual' message
uint8_t WiLEDProto::sendMessageSetIndividual(uint8_t inOutput, uint16_t inAddress){
  __setTypeByte(WiLP_Set_Individual);
  __setDestinationByte(0xFFFF);

  // Set target value
  __setPayloadByte(0, inOutput);
  // Use right-shift to break address into big-endian bytes
  __setPayloadByte(1, (inAddress >> 8));
  __setPayloadByte(2, (inAddress));

  // Store the message length. 10 bytes header, 3 bytes payload, 2 bytes checksum
  __outgoing_message_length = 15;

  return WiLP_RETURN_SUCCESS;
}
////////////////////////////////////////////////////////////////////////////////
// END 'sendMessageTYPE' section
////////////////////////////////////////////////////////////////////////////////
// BEGIN 'setCallbackTYPE' section
//
// Each WiLP message type has a 'setCallbackTYPE' function. This stores a
// callback that enables the WiLP handler to communicate received changes back
// to the main program. The callback functions must return void and accept the
// relevant arguments for that message type (usually source address and payload
// value).
//
void WiLEDProto::setCallbackBeacon(void (*inCBBeacon)(uint16_t, uint32_t)){
  __handler_cb_beacon = inCBBeacon;
}
//
void WiLEDProto::setCallbackSetIndividual(void (*inCBSetIndv)(WiLEDStatus)){
  __handler_cb_set_output = inCBSetIndv;
}
//
////////////////////////////////////////////////////////////////////////////////
// END 'setCallbackTYPE' section
////////////////////////////////////////////////////////////////////////////////
// BEGIN '__handleTypeTYPE' section
//
// These private member functions will be run when handleLastMessage() is called
// and will do whatever processing is needed by that message type, and possibly
// run the callback functions defined in the 'setCallbackTYPE' section (if one
// is defined).
//
/// Handle a 'Beacon' message
void WiLEDProto::__handleTypeBeacon(){
  if(__handler_cb_beacon != NULL){
    // Convert the big-endian payload into the uptime value
    uint32_t uptime;
    uptime =  (__last_received_payload[0] << 24);
    uptime += (__last_received_payload[1] << 16);
    uptime += (__last_received_payload[2] << 8);
    uptime += (__last_received_payload[3]);
    // Call the callback, with the source address and the payload value
    __handler_cb_beacon(__last_received_source, uptime);
  }
}
//
// Handle a 'Set Individual' message
void WiLEDProto::__handleTypeSetIndividual(){
  // Extract the target address from the payload
  uint16_t address;
  address =  (__last_received_payload[1] << 8);
  address += (__last_received_payload[2]);
  // If this handler matches the target address, set it to the target level
  if(address == __self_status.address){
    // Extract the target level from the payload and set it
    uint8_t level = __last_received_payload[0];
    __self_status.level = level;
    // If we have a callback available, call it
    if(__handler_cb_set_output != NULL){
      __handler_cb_set_output(__self_status);
    }
  }
}
//
// Handle a 'Set Two Individuals' messages
void WiLEDProto::__handleTypeSetTwoIndividuals(){
  // Extract the target addresses from the payload
  uint16_t address1;
  address1 =  (__last_received_payload[1] << 8);
  address1 += (__last_received_payload[2]);
  uint16_t address2;
  address2 =  (__last_received_payload[3] << 8);
  address2 += (__last_received_payload[4]);
  // If this handler matches the target address, set it to the target level
  if ((address1 == __self_status.address)
   || (address2 == __self_status.address)){
    // Extract the target level from the payload and set it
    uint8_t level = __last_received_payload[0];
    __self_status.level = level;
    // If we have a callback available, call it
    if(__handler_cb_set_output != NULL){
      __handler_cb_set_output(__self_status);
    }
  }
}
//
// Handle a 'Set Three Individuals' messages
void WiLEDProto::__handleTypeSetThreeIndividuals(){
  // Extract the target addresses from the payload
  uint16_t address1;
  address1 =  (__last_received_payload[1] << 8);
  address1 += (__last_received_payload[2]);
  uint16_t address2;
  address2 =  (__last_received_payload[3] << 8);
  address2 += (__last_received_payload[4]);
  uint16_t address3;
  address3 =  (__last_received_payload[5] << 8);
  address3 += (__last_received_payload[6]);
  // If this handler matches the target address, set it to the target level
  if ((address1 == __self_status.address)
   || (address2 == __self_status.address)
   || (address3 == __self_status.address)){
    // Extract the target level from the payload and set it
    uint8_t level = __last_received_payload[0];
    __self_status.level = level;
    // If we have a callback available, call it
    if(__handler_cb_set_output != NULL){
      __handler_cb_set_output(__self_status);
    }
  }
}
////////////////////////////////////////////////////////////////////////////////
// END '__handleTypeTYPE' section
////////////////////////////////////////////////////////////////////////////////
// BEGIN getter section
//
// Boilerplate C++ class 'getter' functions
//
uint8_t WiLEDProto::getLastReceivedType(){
  return __last_received_type;
}
//
uint16_t WiLEDProto::getLastReceivedSource(){
  return __last_received_source;
}
//
uint16_t WiLEDProto::getLastReceivedDestination(){
  return __last_received_destination;
}
//
uint16_t WiLEDProto::getLastReceivedResetCounter(){
  return __last_received_reset_counter;
}
//
uint16_t WiLEDProto::getLastReceivedMessageCounter(){
  return __last_received_message_counter;
}
//
uint8_t WiLEDProto::getLastReceivedMessageCounterValidation(){
  return __last_received_message_counter_validation;
}
//
////////////////////////////////////////////////////////////////////////////////
// END getter section
////////////////////////////////////////////////////////////////////////////////


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
  // TODO: Use some cleverness here to count the number of payload bytes that
  // have been set, so we always know the current total message length
}


void WiLEDProto::__wipeLastReceived(){
  __last_received_destination = 0;
  __last_received_source = 0;
  __last_received_type = 0;
  __last_received_reset_counter = 0;
  __last_received_message_counter = 0;
}


uint8_t WiLEDProto::__restoreFromStorage_uint16t(uint16_t* outArray, uint16_t inStorageOffset, uint16_t inLength){
  #ifdef ARDUINO_DEBUG
    Serial.println("Reading to storage: ");
    Serial.println((int)(void*)outArray, HEX);
    Serial.println(inStorageOffset);
    Serial.println(inLength);
  #endif
  // Make a 1-byte pointer to the array of 2-byte values
  uint8_t* p = (uint8_t*)(void*)outArray;
  // First, check callback has been set
  if(__storage_read_callback != NULL){
    for (uint16_t idx = 0; idx < inLength; idx++){
      // Read from the storage location into the array
      p[idx] = (*__storage_read_callback)(idx + inStorageOffset);
    }
    return WiLP_RETURN_SUCCESS;
  } else {
    // If callback not set, return with an error
    return WiLP_RETURN_NOT_INIT;
  }
}


uint8_t WiLEDProto::__addToStorage_uint16t(uint16_t* inArray, uint16_t inStorageOffset, uint16_t inLength){
  #ifdef ARDUINO_DEBUG
    Serial.println("Adding to storage: ");
    Serial.println((int)(void*)inArray, HEX);
    Serial.println(*inArray);
    Serial.println(inStorageOffset);
    Serial.println(inLength);
  #endif
  // Make a 1-byte pointer to the array of 2-byte values
  uint8_t* p = (uint8_t*)(void*)inArray;
  // First, check callback has been set
  if(__storage_write_callback != NULL){
    for (uint16_t idx = 0; idx < inLength; idx++){
      // Write from the array into the storage location
      (*__storage_write_callback)(idx + inStorageOffset, p[idx]);
    }
    //(*__storage_commit_callback)();
    return WiLP_RETURN_SUCCESS;
  } else {
    // If callback not set, return with an error
    return WiLP_RETURN_NOT_INIT;
  }
}


uint8_t WiLEDProto::__checkAndUpdateMessageCounter(uint16_t inAddress, uint16_t inResetCounter, uint16_t inMessageCounter){
  // Loop over all the known stored addresses
  for(uint16_t idx = 0; idx < __count_addresses; idx++){
    // Look for the requested address
    if(__address_array[idx] == inAddress){
      // Stored reset counter must be less than or equal to the input reset counter
      if(__reset_counter_array[idx] < inResetCounter){
        // If less than input value, save new values
        __reset_counter_array[idx] = inResetCounter;
        __addToStorage_uint16t(__address_array, STORAGE_ADDRESSES_LOCATION, sizeof(__address_array));
        __addToStorage_uint16t(__reset_counter_array, STORAGE_RESET_LOCATION, sizeof(__reset_counter_array));
        __storage_commit_callback();
        __message_counter_array[idx] = inMessageCounter;
        // Message is fully valid so return success
        return WiLP_RETURN_SUCCESS;
      } else if (__reset_counter_array[idx] != inResetCounter){
        // If input reset counter is less than stored counter, return an error code
        return WiLP_RETURN_INVALID_RST_CTR;
      }

      // Stored message counter must be less than the input message counter
      if(__message_counter_array[idx] < inMessageCounter){
        // If valid, update the stored message counter
        __message_counter_array[idx] = inMessageCounter;
        return WiLP_RETURN_SUCCESS;
      }
      else {
        // Message is invalid, ignore it
        return WiLP_RETURN_INVALID_MSG_CTR;
      }
    }
  }
  // If we reach this point, we did not previously know the address
  // Add the address to our known addresses
  if(__count_addresses < MAXIMUM_STORED_ADDRESSES){
    __address_array[__count_addresses] = inAddress;
    __message_counter_array[__count_addresses] = inMessageCounter;
    __reset_counter_array[__count_addresses] = inResetCounter;
    // Increment the counter
    __count_addresses++;
    // Save the new __address_array and __count_addresses to storage
    // TODO: Check return value of these
    __addToStorage_uint16t(__address_array, STORAGE_ADDRESSES_LOCATION, sizeof(__address_array));
    __addToStorage_uint16t(__reset_counter_array, STORAGE_RESET_LOCATION, sizeof(__reset_counter_array));
    __addToStorage_uint16t(&__count_addresses, STORAGE_COUNT_LOCATION, sizeof(__count_addresses));
    __storage_commit_callback();
    return WiLP_RETURN_ADDED_ADDRESS;
  } else {
    // At maximum known addresses!
    return WiLP_RETURN_AT_MAX_ADDRESSES;
  }
  // We should never get here, but just in case
  return WiLP_RETURN_OTHER_ERROR;
}
