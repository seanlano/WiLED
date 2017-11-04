/*
* WiLEDProto unit test program
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

#include "lib/WiLEDProto/WiLEDProto.h"
#include "gtest/gtest.h"

namespace {

// Blank "storage read" function to pass to the WiLP initialiser
uint8_t BlankReader(uint16_t inAddress){
  uint16_t a = inAddress; // Just to avoid compiler warnings
  a++; // Just to avoid compiler warnings
  return 0;
}
// Blank "storage write" function to pass to the WiLP initialiser
void BlankWriter(uint16_t inAddress, uint8_t inValue){
  uint16_t a = inAddress; // Just to avoid compiler warnings
  a++; // Just to avoid compiler warnings
  uint8_t v = inValue; // Just to avoid compiler warnings
  v++; // Just to avoid compiler warnings
}
// Blank "storage commit" function to pass to the WiLP initialiser
void BlankCommitter(){
  uint8_t a = 0; // Just to avoid compiler warnings
  a++; // Just to avoid compiler warnings
}

// Test fixture to check WiLEDProto.processMessage returns correct values
class ProcessMessageTest : public testing::Test {
  protected:
    // Class instances within a class need to be initialised in the top-level
    // constructor - not in the declaration (hence this weird syntax)
    ProcessMessageTest() :
      p1(0x1000, &BlankReader, &BlankWriter, &BlankCommitter),
      p2(0x2000, &BlankReader, &BlankWriter, &BlankCommitter),
      p3(0x3000, &BlankReader, &BlankWriter, &BlankCommitter) {}

    // Declare some WiLP class hanlders to test
    WiLEDProto p1;
    WiLEDProto p2;
    WiLEDProto p3;

    // Set up the test fixture
    virtual void SetUp() {
      p1.initStorage();
      p2.initStorage();
      p3.initStorage();
    }

    //virtual void TearDown() {}
};

/// Test that the class is empty after being initialised
TEST_F(ProcessMessageTest, IsEmptyInitially) {
  EXPECT_EQ(p1.getLastReceivedResetCounter(), 0);
  EXPECT_EQ(p1.getLastReceivedMessageCounter(), 0);
  EXPECT_EQ(p1.getLastReceivedSource(), 0);
  EXPECT_EQ(p1.getLastReceivedDestination(), 0);
  EXPECT_EQ(p1.getLastReceivedType(), 0);
  EXPECT_EQ(p1.getLastReceivedMessageCounterValidation(), 0);
}

/// Test that the class correctly identifies an invalid message
TEST_F(ProcessMessageTest, IdentifyInvalidMessage) {
  uint8_t invalid_message[MAXIMUM_MESSAGE_LENGTH] = {0};
  // Set magic number to 0x01 (i.e. invalid type)
  invalid_message[0] = 0x01;
  // Set source address to 0x1000 (big endian)
  invalid_message[1] = 0x10;
  invalid_message[2] = 0x00;
  // Set destination address to 0xFFFF
  invalid_message[3] = 0xFF;
  invalid_message[4] = 0xFF;

  // Check the message is processed and flagged as invalid
  ASSERT_EQ(WiLP_RETURN_INVALID_BUFFER, p1.processMessage(invalid_message));
  // Check the "getLast" calls are also set to zero after invalid message
  EXPECT_EQ(p1.getLastReceivedResetCounter(), 0);
  EXPECT_EQ(p1.getLastReceivedMessageCounter(), 0);
  EXPECT_EQ(p1.getLastReceivedSource(), 0);
  EXPECT_EQ(p1.getLastReceivedDestination(), 0);
  EXPECT_EQ(p1.getLastReceivedType(), 0);

  // Test another magic number
  invalid_message[0] = 0xFF;
  ASSERT_EQ(WiLP_RETURN_INVALID_BUFFER, p2.processMessage(invalid_message));
  // Check the "getLast" calls are also set to zero after invalid message
  EXPECT_EQ(p2.getLastReceivedResetCounter(), 0);
  EXPECT_EQ(p2.getLastReceivedMessageCounter(), 0);
  EXPECT_EQ(p2.getLastReceivedSource(), 0);
  EXPECT_EQ(p2.getLastReceivedDestination(), 0);
  EXPECT_EQ(p2.getLastReceivedType(), 0);
}

/// Check the class correctly sets its reset counter
TEST_F(ProcessMessageTest, CorrectFirstResetCounter) {
  // Create a buffer to store the output message
  uint8_t p1_buffer[MAXIMUM_MESSAGE_LENGTH] = {0};
  // Choose a value for "millis()" to pass to the sender
  const uint32_t p1_millis = 1234567890;
  // Process the message and copy it to a buffer
  p1.sendMessageBeacon(p1_millis);
  p1.copyToBuffer(p1_buffer);
  // Pull out the reset counter from the buffer
  uint16_t p1_reset_counter = 0;
  // Bytes 5 and 6 are the reset counter
  p1_reset_counter = (p1_buffer[5] << 8);
  p1_reset_counter += p1_buffer[6];
  // Reset counter should be 1 (since we just initialised p1)
  ASSERT_EQ(p1_reset_counter, 1);
}

/// Check the class correctly sets its message counter
TEST_F(ProcessMessageTest, CorrectFirstMessageCounter) {
  // Create a buffer to store the output message
  uint8_t p1_buffer[MAXIMUM_MESSAGE_LENGTH] = {0};
  // Choose a value for "millis()" to pass to the sender
  const uint32_t p1_millis = 1234567890;
  // Process the message and copy it to a buffer
  p1.sendMessageBeacon(p1_millis);
  p1.copyToBuffer(p1_buffer);
  // Pull out the message counter from the buffer
  uint16_t p1_message_counter = 0;
  // Bytes 7 and 8 are the message counter
  p1_message_counter = (p1_buffer[7] << 8);
  p1_message_counter += p1_buffer[8];
  // Reset counter should be 1 (since we just initialised p1)
  ASSERT_EQ(p1_message_counter, 1);

  // Pull out the reset counter from the buffer
  uint16_t p1_reset_counter = 0;
  // Bytes 5 and 6 are the reset counter
  p1_reset_counter = (p1_buffer[5] << 8);
  p1_reset_counter += p1_buffer[6];
  // Reset counter should be 1 (since we just initialised p1)
  ASSERT_EQ(p1_reset_counter, 1);
}

/// Check the class correctly sets its message counter
TEST_F(ProcessMessageTest, Correct254MessageCounter) {
  const uint16_t number_runs = 254;
  // Create a buffer to store the output message
  uint8_t p1_buffer[MAXIMUM_MESSAGE_LENGTH] = {0};
  // Choose a value for "millis()" to pass to the sender
  const uint32_t p1_millis = 1234567890;
  // Loop for 254 times, to increment the message counter
  for (uint16_t message_counter = 0; message_counter < number_runs; message_counter++)
  {
    p1.sendMessageBeacon(p1_millis);
    p1.copyToBuffer(p1_buffer);
  }

  // Pull out the message counter from the buffer
  uint16_t p1_message_counter = 0;
  // Bytes 7 and 8 are the message counter
  p1_message_counter = (p1_buffer[7] << 8);
  p1_message_counter += p1_buffer[8];
  // Do the actual comparison
  ASSERT_EQ(p1_message_counter, number_runs);

  // Pull out the reset counter from the buffer
  uint16_t p1_reset_counter = 0;
  // Bytes 5 and 6 are the reset counter
  p1_reset_counter = (p1_buffer[5] << 8);
  p1_reset_counter += p1_buffer[6];
  // Reset counter should be 1 (since we just initialised p1)
  ASSERT_EQ(p1_reset_counter, 1);
}

/// Check the class correctly sets its message counter
TEST_F(ProcessMessageTest, Correct1000MessageCounter) {
  const uint16_t number_runs = 1000;
  // Create a buffer to store the output message
  uint8_t p1_buffer[MAXIMUM_MESSAGE_LENGTH] = {0};
  // Choose a value for "millis()" to pass to the sender
  const uint32_t p1_millis = 1234567890;
  // Loop for 1000 times, to increment the message counter
  for (uint16_t message_counter = 0; message_counter < number_runs; message_counter++)
  {
    p1.sendMessageBeacon(p1_millis);
    p1.copyToBuffer(p1_buffer);
  }
  // Pull out the message counter from the buffer
  uint16_t p1_message_counter = 0;
  // Bytes 7 and 8 are the message counter
  p1_message_counter = (p1_buffer[7] << 8);
  p1_message_counter += p1_buffer[8];
  // Do the actual comparison
  ASSERT_EQ(p1_message_counter, number_runs);

  // Pull out the reset counter from the buffer
  uint16_t p1_reset_counter = 0;
  // Bytes 5 and 6 are the reset counter
  p1_reset_counter = (p1_buffer[5] << 8);
  p1_reset_counter += p1_buffer[6];
  // Reset counter should be 1 (since we just initialised p1)
  ASSERT_EQ(p1_reset_counter, 1);
}

/// Check the class correctly sets its message counter
TEST_F(ProcessMessageTest, Correct65535MessageCounter) {
  const uint16_t number_runs = 65535;
  // Create a buffer to store the output message
  uint8_t p1_buffer[MAXIMUM_MESSAGE_LENGTH] = {0};
  // Choose a value for "millis()" to pass to the sender
  const uint32_t p1_millis = 1234567890;
  // Loop for 65535 times, to increment the message counter
  for (uint16_t message_counter = 0; message_counter < number_runs; message_counter++)
  {
    p1.sendMessageBeacon(p1_millis);
    p1.copyToBuffer(p1_buffer);
  }
  // Pull out the message counter from the buffer
  uint16_t p1_message_counter = 0;
  // Bytes 7 and 8 are the message counter
  p1_message_counter = (p1_buffer[7] << 8);
  p1_message_counter += p1_buffer[8];
  // Do the actual comparison
  ASSERT_EQ(p1_message_counter, number_runs);

  // Pull out the reset counter from the buffer
  uint16_t p1_reset_counter = 0;
  // Bytes 5 and 6 are the reset counter
  p1_reset_counter = (p1_buffer[5] << 8);
  p1_reset_counter += p1_buffer[6];
  // Reset counter should be 1 (since we just initialised p1)
  ASSERT_EQ(p1_reset_counter, 1);
}

/// Check the class correctly sets its message counter (after overflow)
TEST_F(ProcessMessageTest, Correct65536MessageCounter) {
  const uint32_t number_runs = 65536;
  // Create a buffer to store the output message
  uint8_t p1_buffer[MAXIMUM_MESSAGE_LENGTH] = {0};
  // Choose a value for "millis()" to pass to the sender
  const uint32_t p1_millis = 1234567890;
  // Loop for 65536 times, to increment the message counter and oveflow it
  for (uint32_t message_counter = 0; message_counter < number_runs; message_counter++)
  {
    p1.sendMessageBeacon(p1_millis);
    p1.copyToBuffer(p1_buffer);
  }
  // Pull out the message counter from the buffer
  uint16_t p1_message_counter = 0;
  // Bytes 7 and 8 are the message counter
  p1_message_counter = (p1_buffer[7] << 8);
  p1_message_counter += p1_buffer[8];
  // Do the actual comparison (should have message_counter = 1 after overflow)
  EXPECT_EQ(p1_message_counter, 1);

  // Pull out the reset counter from the buffer
  uint16_t p1_reset_counter = 0;
  // Bytes 5 and 6 are the reset counter
  p1_reset_counter = (p1_buffer[5] << 8);
  p1_reset_counter += p1_buffer[6];
  // Reset counter should be 2 (since we just overflowed p1)
  EXPECT_EQ(p1_reset_counter, 2);
}

/// Check the class correctly sets its message counter (after overflow)
TEST_F(ProcessMessageTest, Correct65537MessageCounter) {
  const uint32_t number_runs = 65537;
  // Create a buffer to store the output message
  uint8_t p1_buffer[MAXIMUM_MESSAGE_LENGTH] = {0};
  // Choose a value for "millis()" to pass to the sender
  const uint32_t p1_millis = 1234567890;
  // Loop for 65537 times, to increment the message counter and oveflow it
  for (uint32_t message_counter = 0; message_counter < number_runs; message_counter++)
  {
    p1.sendMessageBeacon(p1_millis);
    p1.copyToBuffer(p1_buffer);
  }
  // Pull out the message counter from the buffer
  uint16_t p1_message_counter = 0;
  // Bytes 7 and 8 are the message counter
  p1_message_counter = (p1_buffer[7] << 8);
  p1_message_counter += p1_buffer[8];
  // Do the actual comparison (should have message_counter = 2, 1 run after overflow)
  EXPECT_EQ(p1_message_counter, 2);

  // Pull out the reset counter from the buffer
  uint16_t p1_reset_counter = 0;
  // Bytes 5 and 6 are the reset counter
  p1_reset_counter = (p1_buffer[5] << 8);
  p1_reset_counter += p1_buffer[6];
  // Reset counter should be 2 (since we overflowed p1)
  EXPECT_EQ(p1_reset_counter, 2);
}

////////////////////////////////////////////////////////////////////////////////
// BEGIN tests for "Beacon" message type, 0x01
////////////////////////////////////////////////////////////////////////////////

/// Check the class correctly receives a Beacon message
TEST_F(ProcessMessageTest, CorrectBeaconMessageReceive) {
  // First, manually create a correct beacon message
  uint8_t valid_message[MAXIMUM_MESSAGE_LENGTH] = {0};
  // Set magic number to 0xAA (i.e. valid type)
  valid_message[0] = 0xAA;
  // Set source address to 0x1001 (big endian)
  valid_message[1] = 0x10;
  valid_message[2] = 0x01;
  // Set destination address to 0xFFFF
  valid_message[3] = 0xFF;
  valid_message[4] = 0xFF;
  // Set reset counter to 1
  valid_message[6] = 0x01;
  // Set message counter to 1
  valid_message[8] = 0x01;
  // Set message type flag to WiLP_Beacon
  const uint8_t beacon_type = WiLP_Beacon;
  valid_message[9] = beacon_type;
  // Set the 4 payload bytes to a big-endian uptime number
  //const uint32_t uptime = 0x499602D2; // Decimal = 1234567890
  valid_message[10] = 0x49;
  valid_message[11] = 0x96;
  valid_message[12] = 0x02;
  valid_message[13] = 0xD2;
  // TODO: Set checksum bits

  // Check the message is received properly
  ASSERT_EQ(p1.processMessage(valid_message), WiLP_RETURN_SUCCESS);
  // Check the "getLast" calls are also valid
  EXPECT_EQ(p1.getLastReceivedResetCounter(), 1);
  EXPECT_EQ(p1.getLastReceivedMessageCounter(), 1);
  EXPECT_EQ(p1.getLastReceivedSource(), 0x1001);
  EXPECT_EQ(p1.getLastReceivedDestination(), 0xFFFF);
  EXPECT_EQ(p1.getLastReceivedType(), beacon_type);
  // TODO: Check uptime is received properly (need to implement first)
}

/// Check the class correctly sends a Beacon message
TEST_F(ProcessMessageTest, CorrectBeaconMessageSend) {
  // First, manually create a correct beacon message in an array
  uint8_t valid_message[MAXIMUM_MESSAGE_LENGTH] = {0};
  // Set magic number to 0xAA (i.e. valid type)
  valid_message[0] = 0xAA;
  // Set source address to 0x1000 (big endian) for instance p1
  valid_message[1] = 0x10;
  valid_message[2] = 0x00;
  // Set destination address to 0xFFFF
  valid_message[3] = 0xFF;
  valid_message[4] = 0xFF;
  // Set reset counter to 1 (this shold be the first message being sent)
  valid_message[6] = 0x01;
  // Set message counter to 1 (this shold be the first message being sent)
  valid_message[8] = 0x01;
  // Set message type flag to WiLP_Beacon
  const uint8_t beacon_type = WiLP_Beacon;
  valid_message[9] = beacon_type;
  // Set the 4 payload bytes to a big-endian uptime number
  const uint32_t uptime = 0x499602D2; // Decimal = 1234567890
  valid_message[10] = 0x49;
  valid_message[11] = 0x96;
  valid_message[12] = 0x02;
  valid_message[13] = 0xD2;
  // TODO: Set checksum bits

  // Create buffer for sent message
  uint8_t p1_buffer[MAXIMUM_MESSAGE_LENGTH] = {0};
  // "Send" message and copy to buffer
  p1.sendMessageBeacon(uptime);
  p1.copyToBuffer(p1_buffer);
  // Check the sent buffer is what we expect it to be
  for(uint8_t idx = 0; idx<MAXIMUM_MESSAGE_LENGTH; idx++)
  {
    EXPECT_EQ(p1_buffer[idx], valid_message[idx]);
  }
}

/// Check the class correctly sends and then another can receive a Beacon message
TEST_F(ProcessMessageTest, CorrectBeaconMessageSendReceive) {
  // First, create a message in p1 with uptime
  const uint32_t uptime = 0x499602D2; // Decimal = 1234567890
  // Create buffer for sent message
  uint8_t p1_buffer[MAXIMUM_MESSAGE_LENGTH] = {0};
  // "Send" message and copy to buffer
  p1.sendMessageBeacon(uptime);
  p1.copyToBuffer(p1_buffer);
  // Store the Beacon message type number
  const uint8_t beacon_type = WiLP_Beacon;

  // Next, check the message from p1 is received properly by p2
  ASSERT_EQ(p2.processMessage(p1_buffer), WiLP_RETURN_SUCCESS);
  // Check the "getLast" calls are also valid
  EXPECT_EQ(p2.getLastReceivedResetCounter(), 1); //p1 was just initialised
  EXPECT_EQ(p2.getLastReceivedMessageCounter(), 1); //p1 was just initialised
  EXPECT_EQ(p2.getLastReceivedSource(), 0x1000); // p1 is 0x1000 address
  EXPECT_EQ(p2.getLastReceivedDestination(), 0xFFFF); // Beacon is a broadcast
  EXPECT_EQ(p2.getLastReceivedType(), beacon_type);
  // TODO: Check uptime is received properly (need to implement first)
}
////////////////////////////////////////////////////////////////////////////////
// END tests for "Beacon" message type
////////////////////////////////////////////////////////////////////////////////

} // end namespace
