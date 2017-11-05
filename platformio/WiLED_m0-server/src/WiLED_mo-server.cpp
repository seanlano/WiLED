/* WiLED_m0-server.cpp
* Part of the "WiLED" project, https://github.com/seanlano/WiLED
* An Arduino/PlatformIO project for controlling a PWM dimmable LED through an
* RFM69 packet radio.
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
*
* NOTE: At this early stage, this code does not do very much - it is
* heavily under development!
*/

#include <SPI.h>
#include <RH_RF69.h>

#include <WiLEDProto.h>
#include <FlashStorage.h>

// Hard-wired pins for Feather M0
#define RFM69_CS      8
#define RFM69_IRQ     3
#define RFM69_RST     4
#define LED           13

// Singleton instance of the radio driver
RH_RF69 rf69(RFM69_CS, RFM69_IRQ);

// Define a struct for storing to flash
typedef struct {
  uint8_t data[4096];
} flash_array;

// Initialise FlashStorage class, with enough space for a flash_array struct
FlashStorage(storage, flash_array);
// Declare an instance of the flash_array struct, called flash_temp
flash_array flash_temp;

// Create function to pass to WiLP class for reading flash storage
uint8_t FlashReader(uint16_t inAddress){
  // Update the in-memory copy, flash_temp, with what is stored in flash memory
  // TODO: Optimise this so read() is not called if flash_temp is already in memory
  flash_temp = storage.read();
  // Return the requested value to the callback
  return flash_temp.data[inAddress];
}
// Create function to pass to WiLP class for writing to flash storage
void FlashWriter(uint16_t inAddress, uint8_t inValue){
  // Update the in-memory copy, flash_temp, with the given value
  flash_temp.data[inAddress] = inValue;
}
// Create function to pass to WiLP class for committing flash storage
void FlashCommitter(){
  // Actually write the in-memory copy to the flash storage
  storage.write(flash_temp);
}


WiLEDProto handler(0x0001, &FlashReader, &FlashWriter, &FlashCommitter);


void setup()
{
  pinMode(LED, OUTPUT);
  Serial1.begin(115200);

  Serial1.println(F("Feather M0 RFM69 RX Test!"));
  Serial1.println();

  if (!rf69.init())
    Serial1.println("init failed");
  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM (for low power module)
  // No encryption
  if (!rf69.setFrequency(915.0))
    Serial1.println("setFrequency failed");

  // If you are using a high power RF69 eg RFM69HW, you *must* set a Tx power with the
  // ishighpowermodule flag set like this:
  rf69.setTxPower(20, true);
  rf69.setModemConfig(RH_RF69::GFSK_Rb250Fd250);

  // The encryption key has to be the same as the one in the client
  uint8_t key[] = { 0xFF, 0xAE, 0x16, 0x03, 0xE1, 0x10, 0x07, 0x03,
                    0x1F, 0x44, 0xE1, 0x66, 0x65, 0x55, 0xF0, 0xB6
                  };
  rf69.setEncryptionKey(key);
  rf69.setCADTimeout(2);

  Serial1.println("About to init flash storage");
  handler.initStorage();
  Serial1.println("Init complete");
}


void loop()
{
  if (rf69.available())
  {
    digitalWrite(LED, HIGH);
    // Should be a message for us now
    uint8_t buf[RH_RF69_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if (rf69.recv(buf, &len))
    {
      Serial1.print("HEX: ");
      for(int idx=0; idx<20; idx++){
        Serial1.print(buf[idx], HEX);
        Serial1.print(" ");
      }
      Serial1.println(". ");

      uint8_t status_code = handler.processMessage(buf);
      int16_t msg_check_code = -1;

      if(status_code == WiLP_RETURN_SUCCESS){
        msg_check_code = handler.getLastReceivedMessageCounterValidation();
      }

      Serial1.print("Message analysis. Return code: ");
      Serial1.println(status_code, DEC);
      Serial1.print("  Source device: ");
      Serial1.println(handler.getLastReceivedSource(), HEX);
      Serial1.print("  Destination device: ");
      Serial1.println(handler.getLastReceivedDestination(), HEX);
      Serial1.print("  Message type: ");
      Serial1.println(handler.getLastReceivedType(), HEX);
      Serial1.print("  Reset counter: ");
      Serial1.println(handler.getLastReceivedResetCounter(), DEC);
      Serial1.print("  Message counter: ");
      Serial1.print(handler.getLastReceivedMessageCounter(), DEC);
      if(msg_check_code == WiLP_RETURN_SUCCESS){
        Serial1.println(" (VALID)");
      } else if(msg_check_code == WiLP_RETURN_ADDED_ADDRESS){
        Serial1.println(" (ADDED NEW)");
      } else if(status_code == WiLP_RETURN_INVALID_RST_CTR){
        Serial1.println(" (INVALID RST)");
      } else if(status_code == WiLP_RETURN_INVALID_MSG_CTR){
        Serial1.println(" (INVALID MSG)");
      } else if(status_code == WiLP_RETURN_INVALID_CHECKSUM){
        Serial1.println(" (CHECKSUM FAILED)");
      } else {
        Serial1.println(" (OTHER ERROR)");
      }
      //Serial1.print("RSSI: ");
      //Serial1.println(rf69.lastRssi(), DEC);
      //Serial1.print("Millis delta with last message: ");
      //Serial1.println(this_message - last_message);
      Serial1.println();

      // Send a reply back
      uint8_t data[MAXIMUM_MESSAGE_LENGTH];
      status_code = handler.sendMessageBeacon(millis());
      handler.copyToBuffer(data);
      delay(2);
      rf69.waitCAD();
      rf69.send(data, MAXIMUM_MESSAGE_LENGTH);
      rf69.waitPacketSent();

      Serial1.println("Sent a reply");
      Serial1.println();
    }
    else
    {
      Serial1.println("recv failed");
    }
    digitalWrite(LED, LOW);
  }
}
