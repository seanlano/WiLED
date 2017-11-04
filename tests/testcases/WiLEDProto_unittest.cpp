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
  EXPECT_EQ(0, p1.getLastReceivedResetCounter());
  EXPECT_EQ(0, p1.getLastReceivedMessageCounter());
  EXPECT_EQ(0, p1.getLastReceivedSource());
  EXPECT_EQ(0, p1.getLastReceivedDestination());
  EXPECT_EQ(0, p1.getLastReceivedType());
  EXPECT_EQ(0, p1.getLastReceivedMessageCounterValidation());
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
  EXPECT_EQ(0, p1.getLastReceivedResetCounter());
  EXPECT_EQ(0, p1.getLastReceivedMessageCounter());
  EXPECT_EQ(0, p1.getLastReceivedSource());
  EXPECT_EQ(0, p1.getLastReceivedDestination());
  EXPECT_EQ(0, p1.getLastReceivedType());

  // Test another magic number
  invalid_message[0] = 0xFF;
  ASSERT_EQ(WiLP_RETURN_INVALID_BUFFER, p2.processMessage(invalid_message));
  // Check the "getLast" calls are also set to zero after invalid message
  EXPECT_EQ(0, p2.getLastReceivedResetCounter());
  EXPECT_EQ(0, p2.getLastReceivedMessageCounter());
  EXPECT_EQ(0, p2.getLastReceivedSource());
  EXPECT_EQ(0, p2.getLastReceivedDestination());
  EXPECT_EQ(0, p2.getLastReceivedType());
}

// Check the class correctly sets its reset counter
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
  ASSERT_EQ(1, p1_reset_counter);
}

// Check the class correctly sets its message counter
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
  ASSERT_EQ(1, p1_message_counter);

  // Pull out the reset counter from the buffer
  uint16_t p1_reset_counter = 0;
  // Bytes 5 and 6 are the reset counter
  p1_reset_counter = (p1_buffer[5] << 8);
  p1_reset_counter += p1_buffer[6];
  // Reset counter should be 1 (since we just initialised p1)
  ASSERT_EQ(1, p1_reset_counter);
}

// Check the class correctly sets its message counter
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
  ASSERT_EQ(number_runs, p1_message_counter);

  // Pull out the reset counter from the buffer
  uint16_t p1_reset_counter = 0;
  // Bytes 5 and 6 are the reset counter
  p1_reset_counter = (p1_buffer[5] << 8);
  p1_reset_counter += p1_buffer[6];
  // Reset counter should be 1 (since we just initialised p1)
  ASSERT_EQ(1, p1_reset_counter);
}

// Check the class correctly sets its message counter
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
  ASSERT_EQ(number_runs, p1_message_counter);

  // Pull out the reset counter from the buffer
  uint16_t p1_reset_counter = 0;
  // Bytes 5 and 6 are the reset counter
  p1_reset_counter = (p1_buffer[5] << 8);
  p1_reset_counter += p1_buffer[6];
  // Reset counter should be 1 (since we just initialised p1)
  ASSERT_EQ(1, p1_reset_counter);
}

// Check the class correctly sets its message counter
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
  ASSERT_EQ(number_runs, p1_message_counter);

  // Pull out the reset counter from the buffer
  uint16_t p1_reset_counter = 0;
  // Bytes 5 and 6 are the reset counter
  p1_reset_counter = (p1_buffer[5] << 8);
  p1_reset_counter += p1_buffer[6];
  // Reset counter should be 1 (since we just initialised p1)
  ASSERT_EQ(1, p1_reset_counter);
}

// Check the class correctly sets its message counter (after overflow)
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
  EXPECT_EQ(1, p1_message_counter);

  // Pull out the reset counter from the buffer
  uint16_t p1_reset_counter = 0;
  // Bytes 5 and 6 are the reset counter
  p1_reset_counter = (p1_buffer[5] << 8);
  p1_reset_counter += p1_buffer[6];
  // Reset counter should be 2 (since we just overflowed p1)
  EXPECT_EQ(2, p1_reset_counter);
}

// Check the class correctly sets its message counter (after overflow)
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
  EXPECT_EQ(2, p1_message_counter);

  // Pull out the reset counter from the buffer
  uint16_t p1_reset_counter = 0;
  // Bytes 5 and 6 are the reset counter
  p1_reset_counter = (p1_buffer[5] << 8);
  p1_reset_counter += p1_buffer[6];
  // Reset counter should be 2 (since we overflowed p1)
  EXPECT_EQ(2, p1_reset_counter);
}

} // end namespace
