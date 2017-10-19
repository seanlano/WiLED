/* WiLED_m0-server.ino
* Part of the "WiLED" project, https://github.com/seanlano/WiLED
* An Arduino project for controlling a PWM dimmable LED through an RFM69
* packet radio. . 
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
#include "lib/LEDOutput.h"
#include "lib/RunMode.h"
#include "lib/Rotary.h"

// Hard-wired pins for Feather M0
#define RFM69_CS      8
#define RFM69_IRQ     3
#define RFM69_RST     4
#define LED           13

// Singleton instance of the radio driver
RH_RF69 rf69(RFM69_CS, RFM69_IRQ); 

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
}

String message = "";

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
      Serial1.print("got request: ");
      Serial1.println((char*)buf);
      Serial1.print("RSSI: ");
      Serial1.println(rf69.lastRssi(), DEC);
      
      message = "Received";

      uint8_t buf2[RH_RF69_MAX_MESSAGE_LEN];
    
      message.toCharArray((char *)buf2, message.length());
      
      // Send a reply
      rf69.waitCAD();
      rf69.send(buf2, message.length());
      rf69.waitPacketSent();
      Serial1.println("Sent a reply");
    }
    else
    {
      Serial1.println("recv failed");
    }
    digitalWrite(LED, LOW);
  }
}

