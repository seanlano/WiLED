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
  _storage_read_callback = inStorageReadCB;
  _storage_write_callback = inStorageWriteCB;
  _storage_commit_callback = inStorageCommitCB;
  // Store the address, must not be 0 or 65535
  if(inAddress > 0 && inAddress < 65535){
    _self_status.address = inAddress;
  } else {
    _self_status.address = 0;
  }
  // Set magic number
  _outgoing_message_buffer[0] = 0xAA;
  // Set source address (as big-endian 2-byte number)
  _outgoing_message_buffer[1] = (_self_status.address >> 8);
  _outgoing_message_buffer[2] = (_self_status.address);
}

/// initStorage() should be called after setting up the storage device in the
/// main program, i.e. once storage callbacks are ready to be used.
void WiLEDProto::initStorage(){

  if(_storage_commit_callback != NULL && _storage_write_callback != NULL){
    // TODO: Check the return value of these?
    // Read the addresses and reset counter arrays from storage
    _restoreFromStorage_uint16t(_address_array, STORAGE_ADDRESSES_LOCATION, sizeof(_address_array));
    _restoreFromStorage_uint16t(_reset_counter_array, STORAGE_RESET_LOCATION, sizeof(_reset_counter_array));
    _restoreFromStorage_uint16t(&_count_addresses, STORAGE_COUNT_LOCATION, sizeof(_count_addresses));

    // Read this devices own reset counter, then increment it and store it
    _restoreFromStorage_uint16t(&_self_reset_counter, STORAGE_SELF_RESET_LOCATION, sizeof(_self_reset_counter));
    _self_reset_counter++;
    _addToStorage_uint16t(&_self_reset_counter, STORAGE_SELF_RESET_LOCATION, sizeof(_self_reset_counter));
    _storage_commit_callback();

    // Arduino-specific debug
    #ifdef ARDUINO_DEBUG
      Serial.print("Loaded addresses: ");
      Serial.println(_count_addresses);
      Serial.print("Addresses: ");
      for(uint16_t idx=0; idx < _count_addresses; idx++){
        Serial.print(_address_array[idx], HEX);
        Serial.print(", ");
      }
      Serial.println();
      Serial.print("This device's reset counter: ");
      Serial.println(_self_reset_counter);
      Serial.println();
    #endif
  }
}


// Process a received message
uint8_t WiLEDProto::processMessage(uint8_t* inBuffer){
  _last_was_valid = false;
  // Check if first byte is magic number
  if(inBuffer[0] != 0xAA){
    _wipeLastReceived();
    return WiLP_RETURN_INVALID_BUFFER;
  }
  // Store the received message destination (left shift)
  _last_received_destination = (inBuffer[3] << 8);
  _last_received_destination += inBuffer[4];
  // Store the received message source (left shift)
  _last_received_source = (inBuffer[1] << 8);
  _last_received_source += inBuffer[2];
  // Store the received message type byte
  _last_received_type = inBuffer[9];
  // Store the received reset counter (left shift)
  _last_received_reset_counter = (inBuffer[5] << 8);
  _last_received_reset_counter += inBuffer[6];
  // Store the received message counter
  _last_received_message_counter = (inBuffer[7] << 8);
  _last_received_message_counter += inBuffer[8];
  // Check if we are the destination
  if((_last_received_destination != _self_status.address) &&
     (_last_received_destination != 0xFFFF)){
    return WiLP_RETURN_NOT_THIS_DEST;
  }

  _last_received_message_counter_validation =
  _checkAndUpdateMessageCounter(_last_received_source,
                                 _last_received_reset_counter,
                                 _last_received_message_counter);

  // Determine the payload length and which process callback to use
  switch(_last_received_type){
    case WiLP_Beacon:
      _last_received_payload_length = 4;
      _process_callback = &WiLEDProto::_handleTypeBeacon;
      break;
    case WiLP_Set_Individual:
      _last_received_payload_length = 3;
      _process_callback = &WiLEDProto::_handleTypeSetIndividual;
      break;
    case WiLP_Set_Two_Individuals:
      _last_received_payload_length = 5;
      _process_callback = &WiLEDProto::_handleTypeSetTwoIndividuals;
      break;
    case WiLP_Set_Three_Individuals:
      _last_received_payload_length = 7;
      _process_callback = &WiLEDProto::_handleTypeSetThreeIndividuals;
      break;
    default:
      _last_received_payload_length = 0;
      _process_callback = NULL;
      return WiLP_RETURN_UNKNOWN_TYPE;
  }
  // Store the payload bytes in an array
  if(_last_received_payload_length > 0){
    memcpy(_last_received_payload, &inBuffer[10], _last_received_payload_length);
  }

  // Calculate the CRC16 checksum of the received message
  uint16_t checksum = CRC16(inBuffer, 0, 10+_last_received_payload_length);
  // Pull out the checksum in the received message
  uint16_t received_checksum;
  received_checksum  = (inBuffer[10+_last_received_payload_length] << 8);
  received_checksum += inBuffer[10+_last_received_payload_length+1];
  // Check the calculated checksum matches the received checksum
  if(checksum != received_checksum){
    return WiLP_RETURN_INVALID_CHECKSUM;
  }

  // If received message does not pass the validation, return with an error code
  if((_last_received_message_counter_validation == WiLP_RETURN_SUCCESS) ||
     (_last_received_message_counter_validation == WiLP_RETURN_ADDED_ADDRESS)){
    // Store the fact that this is a valid message
    _last_was_valid = true;
    // Return to caller with success
    return WiLP_RETURN_SUCCESS;
  } else {
    return _last_received_message_counter_validation;
  }
}


// Handle the callback for the last received message, if there is one
void WiLEDProto::handleLastMessage(){
  if((_process_callback != NULL) && (_last_was_valid == true)){
    // Run the callback, with funky C++ syntax for calling a member-function pointer
    (this->*_process_callback)();
  }
}


void WiLEDProto::copyToBuffer(uint8_t * inBuffer){
  /// copyToBuffer can only be called once. After calling, the
  /// message contents must be set again.

  // Increment and set message counter bytes (big endian)
  // If at maximum for uint16_t, increment reset counter and go back to zero
  if(_self_message_counter == 65535){
    _self_message_counter = 0;
    // Increment the reset counter and store it
    _self_reset_counter++;
    _addToStorage_uint16t(&_self_reset_counter,
                           STORAGE_SELF_RESET_LOCATION,
                           sizeof(_self_reset_counter));
    _storage_commit_callback();
  }
  _self_message_counter++;
  _outgoing_message_buffer[7] = (_self_message_counter >> 8);
  _outgoing_message_buffer[8] = (_self_message_counter);

  // Set reset counter bytes
  _outgoing_message_buffer[5] = (_self_reset_counter >> 8);
  _outgoing_message_buffer[6] = (_self_reset_counter);

  // Calculate and insert CRC16 checksum
  uint16_t checksum = CRC16(_outgoing_message_buffer,
                            0,
                            _outgoing_message_length-2);
  // Store the checksum in the last 2 bytes of the message
  _outgoing_message_buffer[_outgoing_message_length-2] = (checksum >> 8);
  _outgoing_message_buffer[_outgoing_message_length-1] = (checksum);

  // Copy internal buffer to provided address
  memcpy(inBuffer, _outgoing_message_buffer, MAXIMUM_MESSAGE_LENGTH);

  // Wipe message buffer (keep magic number and source address)
  for(uint8_t idx = 3; idx<MAXIMUM_MESSAGE_LENGTH; idx++){
    _outgoing_message_buffer[idx] = 0x00;
  }
  _outgoing_message_length = 0;
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
  _setTypeByte(WiLP_Beacon);
  _setDestinationByte(0xFFFF);

  // Use right-shift to break into four (big endian) 1-byte blocks
  _setPayloadByte(0, (inUptime >> 24));
  _setPayloadByte(1, (inUptime >> 16));
  _setPayloadByte(2, (inUptime >> 8));
  _setPayloadByte(3, (inUptime));

  // Store the message length. 10 bytes header, 4 bytes payload, 2 bytes checksum
  _outgoing_message_length = 16;

  return WiLP_RETURN_SUCCESS;
}
//
/// Send a 'Set Individual' message
uint8_t WiLEDProto::sendMessageSetIndividual(uint8_t inOutput, uint16_t inAddress){
  _setTypeByte(WiLP_Set_Individual);
  _setDestinationByte(0xFFFF);

  // Set target value
  _setPayloadByte(0, inOutput);
  // Use right-shift to break address into big-endian bytes
  _setPayloadByte(1, (inAddress >> 8));
  _setPayloadByte(2, (inAddress));

  // Store the message length. 10 bytes header, 3 bytes payload, 2 bytes checksum
  _outgoing_message_length = 15;

  return WiLP_RETURN_SUCCESS;
}
/// Send a 'Set Two Individuals' message
uint8_t WiLEDProto::sendMessageSetTwoIndividuals(uint8_t inOutput, uint16_t inAddress1, uint16_t inAddress2){
  _setTypeByte(WiLP_Set_Two_Individuals);
  _setDestinationByte(0xFFFF);

  // Set target value
  _setPayloadByte(0, inOutput);
  // Use right-shift to break address into big-endian bytes
  _setPayloadByte(1, (inAddress1 >> 8));
  _setPayloadByte(2, (inAddress1));
  _setPayloadByte(3, (inAddress2 >> 8));
  _setPayloadByte(4, (inAddress2));

  // Store the message length. 10 bytes header, 5 bytes payload, 2 bytes checksum
  _outgoing_message_length = 17;

  return WiLP_RETURN_SUCCESS;
}
/// Send a 'Set Three Individuals' message
uint8_t WiLEDProto::sendMessageSetThreeIndividuals(uint8_t inOutput, uint16_t inAddress1, uint16_t inAddress2, uint16_t inAddress3){
  _setTypeByte(WiLP_Set_Three_Individuals);
  _setDestinationByte(0xFFFF);

  // Set target value
  _setPayloadByte(0, inOutput);
  // Use right-shift to break address into big-endian bytes
  _setPayloadByte(1, (inAddress1 >> 8));
  _setPayloadByte(2, (inAddress1));
  _setPayloadByte(3, (inAddress2 >> 8));
  _setPayloadByte(4, (inAddress2));
  _setPayloadByte(5, (inAddress3 >> 8));
  _setPayloadByte(6, (inAddress3));

  // Store the message length. 10 bytes header, 7 bytes payload, 2 bytes checksum
  _outgoing_message_length = 19;

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
  _handler_cb_beacon = inCBBeacon;
}
//
void WiLEDProto::setCallbackSetIndividual(void (*inCBSetIndv)(WiLEDStatus)){
  _handler_cb_set_output = inCBSetIndv;
}
//
////////////////////////////////////////////////////////////////////////////////
// END 'setCallbackTYPE' section
////////////////////////////////////////////////////////////////////////////////
// BEGIN '_handleTypeTYPE' section
//
// These private member functions will be run when handleLastMessage() is called
// and will do whatever processing is needed by that message type, and possibly
// run the callback functions defined in the 'setCallbackTYPE' section (if one
// is defined).
//
/// Handle a 'Beacon' message
void WiLEDProto::_handleTypeBeacon(){
  if(_handler_cb_beacon != NULL){
    // Convert the big-endian payload into the uptime value
    uint32_t uptime;
    uptime =  (_last_received_payload[0] << 24);
    uptime += (_last_received_payload[1] << 16);
    uptime += (_last_received_payload[2] << 8);
    uptime += (_last_received_payload[3]);
    // Call the callback, with the source address and the payload value
    _handler_cb_beacon(_last_received_source, uptime);
  }
}
//
// Handle a 'Set Individual' message
void WiLEDProto::_handleTypeSetIndividual(){
  // Extract the target address from the payload
  uint16_t address;
  address =  (_last_received_payload[1] << 8);
  address += (_last_received_payload[2]);
  // If this handler matches the target address, set it to the target level
  if(address == _self_status.address){
    // Extract the target level from the payload and set it
    uint8_t level = _last_received_payload[0];
    _self_status.level = level;
    // If we have a callback available, call it
    if(_handler_cb_set_output != NULL){
      _handler_cb_set_output(_self_status);
    }
  }
}
//
// Handle a 'Set Two Individuals' messages
void WiLEDProto::_handleTypeSetTwoIndividuals(){
  // Extract the target addresses from the payload
  uint16_t address1;
  address1 =  (_last_received_payload[1] << 8);
  address1 += (_last_received_payload[2]);
  uint16_t address2;
  address2 =  (_last_received_payload[3] << 8);
  address2 += (_last_received_payload[4]);
  // If this handler matches the target address, set it to the target level
  if ((address1 == _self_status.address)
   || (address2 == _self_status.address)){
    // Extract the target level from the payload and set it
    uint8_t level = _last_received_payload[0];
    _self_status.level = level;
    // If we have a callback available, call it
    if(_handler_cb_set_output != NULL){
      _handler_cb_set_output(_self_status);
    }
  }
}
//
// Handle a 'Set Three Individuals' messages
void WiLEDProto::_handleTypeSetThreeIndividuals(){
  // Extract the target addresses from the payload
  uint16_t address1;
  address1 =  (_last_received_payload[1] << 8);
  address1 += (_last_received_payload[2]);
  uint16_t address2;
  address2 =  (_last_received_payload[3] << 8);
  address2 += (_last_received_payload[4]);
  uint16_t address3;
  address3 =  (_last_received_payload[5] << 8);
  address3 += (_last_received_payload[6]);
  // If this handler matches the target address, set it to the target level
  if ((address1 == _self_status.address)
   || (address2 == _self_status.address)
   || (address3 == _self_status.address)){
    // Extract the target level from the payload and set it
    uint8_t level = _last_received_payload[0];
    _self_status.level = level;
    // If we have a callback available, call it
    if(_handler_cb_set_output != NULL){
      _handler_cb_set_output(_self_status);
    }
  }
}
////////////////////////////////////////////////////////////////////////////////
// END '_handleTypeTYPE' section
////////////////////////////////////////////////////////////////////////////////
// BEGIN getter section
//
// Boilerplate C++ class 'getter' functions
//
uint8_t WiLEDProto::getLastReceivedType(){
  return _last_received_type;
}
//
uint16_t WiLEDProto::getLastReceivedSource(){
  return _last_received_source;
}
//
uint16_t WiLEDProto::getLastReceivedDestination(){
  return _last_received_destination;
}
//
uint16_t WiLEDProto::getLastReceivedResetCounter(){
  return _last_received_reset_counter;
}
//
uint16_t WiLEDProto::getLastReceivedMessageCounter(){
  return _last_received_message_counter;
}
//
uint8_t WiLEDProto::getLastReceivedMessageCounterValidation(){
  return _last_received_message_counter_validation;
}
//
////////////////////////////////////////////////////////////////////////////////
// END getter section
////////////////////////////////////////////////////////////////////////////////


// Set the "type" byte in the output buffer
void WiLEDProto::_setTypeByte(uint8_t inType){
  _outgoing_message_buffer[9] = inType;
}


// Set the "destination" bytes in the output buffer
void WiLEDProto::_setDestinationByte(uint16_t inDestination){
  // Set destination address (as big-endian 2-byte number)
  _outgoing_message_buffer[3] = (inDestination >> 8);
  _outgoing_message_buffer[4] = (inDestination);
}


// Set the specified "payload" byte in the output buffer
void WiLEDProto::_setPayloadByte(uint8_t inPayloadOffset, uint8_t inPayloadValue){
  _outgoing_message_buffer[10+inPayloadOffset] = inPayloadValue;
  // TODO: Use some cleverness here to count the number of payload bytes that
  // have been set, so we always know the current total message length
}


void WiLEDProto::_wipeLastReceived(){
  _last_received_destination = 0;
  _last_received_source = 0;
  _last_received_type = 0;
  _last_received_reset_counter = 0;
  _last_received_message_counter = 0;
}


uint8_t WiLEDProto::_restoreFromStorage_uint16t(uint16_t* outArray, uint16_t inStorageOffset, uint16_t inLength){
  #ifdef ARDUINO_DEBUG
    Serial.println("Reading to storage: ");
    Serial.println((int)(void*)outArray, HEX);
    Serial.println(inStorageOffset);
    Serial.println(inLength);
  #endif
  // Make a 1-byte pointer to the array of 2-byte values
  uint8_t* p = (uint8_t*)(void*)outArray;
  // First, check callback has been set
  if(_storage_read_callback != NULL){
    for (uint16_t idx = 0; idx < inLength; idx++){
      // Read from the storage location into the array
      p[idx] = (*_storage_read_callback)(idx + inStorageOffset);
    }
    return WiLP_RETURN_SUCCESS;
  } else {
    // If callback not set, return with an error
    return WiLP_RETURN_NOT_INIT;
  }
}


uint8_t WiLEDProto::_addToStorage_uint16t(uint16_t* inArray, uint16_t inStorageOffset, uint16_t inLength){
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
  if(_storage_write_callback != NULL){
    for (uint16_t idx = 0; idx < inLength; idx++){
      // Write from the array into the storage location
      (*_storage_write_callback)(idx + inStorageOffset, p[idx]);
    }
    //(*_storage_commit_callback)();
    return WiLP_RETURN_SUCCESS;
  } else {
    // If callback not set, return with an error
    return WiLP_RETURN_NOT_INIT;
  }
}


uint8_t WiLEDProto::_checkAndUpdateMessageCounter(uint16_t inAddress, uint16_t inResetCounter, uint16_t inMessageCounter){
  // Loop over all the known stored addresses
  for(uint16_t idx = 0; idx < _count_addresses; idx++){
    // Look for the requested address
    if(_address_array[idx] == inAddress){
      // Stored reset counter must be less than or equal to the input reset counter
      if(_reset_counter_array[idx] < inResetCounter){
        // If less than input value, save new values
        _reset_counter_array[idx] = inResetCounter;
        _addToStorage_uint16t(_address_array, STORAGE_ADDRESSES_LOCATION, sizeof(_address_array));
        _addToStorage_uint16t(_reset_counter_array, STORAGE_RESET_LOCATION, sizeof(_reset_counter_array));
        _storage_commit_callback();
        _message_counter_array[idx] = inMessageCounter;
        // Message is fully valid so return success
        return WiLP_RETURN_SUCCESS;
      } else if (_reset_counter_array[idx] != inResetCounter){
        // If input reset counter is less than stored counter, return an error code
        return WiLP_RETURN_INVALID_RST_CTR;
      }

      // Stored message counter must be less than the input message counter
      if(_message_counter_array[idx] < inMessageCounter){
        // If valid, update the stored message counter
        _message_counter_array[idx] = inMessageCounter;
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
  if(_count_addresses < MAXIMUM_STORED_ADDRESSES){
    _address_array[_count_addresses] = inAddress;
    _message_counter_array[_count_addresses] = inMessageCounter;
    _reset_counter_array[_count_addresses] = inResetCounter;
    // Increment the counter
    _count_addresses++;
    // Save the new _address_array and _count_addresses to storage
    // TODO: Check return value of these
    _addToStorage_uint16t(_address_array, STORAGE_ADDRESSES_LOCATION, sizeof(_address_array));
    _addToStorage_uint16t(_reset_counter_array, STORAGE_RESET_LOCATION, sizeof(_reset_counter_array));
    _addToStorage_uint16t(&_count_addresses, STORAGE_COUNT_LOCATION, sizeof(_count_addresses));
    _storage_commit_callback();
    return WiLP_RETURN_ADDED_ADDRESS;
  } else {
    // At maximum known addresses!
    return WiLP_RETURN_AT_MAX_ADDRESSES;
  }
  // We should never get here, but just in case
  return WiLP_RETURN_OTHER_ERROR;
}
