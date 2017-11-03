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

// Check the class is empty after being initialised
TEST_F(ProcessMessageTest, IsEmptyInitially) {
  EXPECT_EQ(0, p1.getLastReceivedResetCounter());
  EXPECT_EQ(0, p1.getLastReceivedMessageCounter());
  EXPECT_EQ(0, p1.getLastReceivedSource());
  EXPECT_EQ(0, p1.getLastReceivedDestination());
  EXPECT_EQ(0, p1.getLastReceivedType());
}

} // end namespace
