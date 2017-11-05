/* WiLED_esp8266-client-pingtest.cpp
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

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <EEPROM.h>

#include <string.h>

#include <SPI.h>
#include <RH_RF69.h>

#include <LEDOutput.h>
#include <RunMode.h>
#include <Rotary.h>
#include <WiLEDProto.h>


#define RFM69_CS      16
#define RFM69_IRQ     15


// Uncomment this #define to enable the "retransmit test" - i.e. send the same message twice.
// This should be detected by any receiver and marked as invalid.
//#define RETRANSMIT_TEST


const uint16_t SERVER_ADDRESS = 0x0001;
const uint16_t CLIENT_ADDRESS = 0x1234;


// We connect to Wi-Fi so that the ESP8266 OTA updates can be used - this makes
// development much easier.
const char* ssid = "SSID"; // Clearly this needs to be changed to the actual SSID
const char* password = "PASS"; // And this needs to be the password


// Singleton instance of the radio driver
RH_RF69 rf69(RFM69_CS, RFM69_IRQ);


// Create function to pass to WiLP class for reading EEPROM storage
uint8_t EEPROMreader(uint16_t inAddress){
  return EEPROM.read(inAddress);
}
// Create function to pass to WiLP class for writing to EEPROM storage
void EEPROMwriter(uint16_t inAddress, uint8_t inValue){
  EEPROM.write(inAddress, inValue);
}
// Create function to pass to WiLP class for committing EEPROM storage
void EEPROMcommitter(){
  EEPROM.commit();
}


WiLEDProto handler(CLIENT_ADDRESS, &EEPROMreader, &EEPROMwriter, &EEPROMcommitter);


uint8_t msg_status = 0;

void sendMessage()
{
  Serial.println(F("Sending to rf69_server with Datagram"));
  uint8_t data[MAXIMUM_MESSAGE_LENGTH];
  uint8_t len = sizeof(data);

  msg_status = handler.sendMessageBeacon(millis());
  handler.copyToBuffer(data);

  // Send a message to rf69_server

  // Send a message to manager_server
  rf69.waitCAD();
  rf69.send(data, MAXIMUM_MESSAGE_LENGTH);
  rf69.waitPacketSent();

  #ifdef RETRANSMIT_TEST
  delay(15);
  rf69.waitCAD();
  rf69.send(data, MAXIMUM_MESSAGE_LENGTH);
  rf69.waitPacketSent();
  #endif

  // Now wait for a reply
  uint8_t buf[RH_RF69_MAX_MESSAGE_LEN];
  len = sizeof(buf);
  // Wait for up to 15 ms for a reply
  if (rf69.waitAvailableTimeout(15))
  {
    // Should be a reply message for us now
    if (rf69.recv(buf, &len))
    {
      Serial.print("HEX: ");
      for(int idx=0; idx<20; idx++){
        Serial.print(buf[idx], HEX);
        Serial.print(" ");
      }
      Serial.println(". ");

      uint8_t status_code = handler.processMessage(buf);
      int16_t msg_check_code = -1;

      if(status_code == WiLP_RETURN_SUCCESS){
        msg_check_code = handler.getLastReceivedMessageCounterValidation();
      }

      Serial.print("Message analysis. Return code: ");
      Serial.println(status_code, DEC);
      Serial.print("  Source device: ");
      Serial.println(handler.getLastReceivedSource(), HEX);
      Serial.print("  Destination device: ");
      Serial.println(handler.getLastReceivedDestination(), HEX);
      Serial.print("  Message type: ");
      Serial.println(handler.getLastReceivedType(), HEX);
      Serial.print("  Reset counter: ");
      Serial.println(handler.getLastReceivedResetCounter(), DEC);
      Serial.print("  Message counter: ");
      Serial.print(handler.getLastReceivedMessageCounter(), DEC);
      if(msg_check_code == WiLP_RETURN_SUCCESS){
        Serial.println(" (VALID)");
      } else if(msg_check_code == WiLP_RETURN_ADDED_ADDRESS){
        Serial.println(" (ADDED NEW)");
      } else if(status_code == WiLP_RETURN_INVALID_RST_CTR){
        Serial.println(" (INVALID RST)");
      } else if(status_code == WiLP_RETURN_INVALID_MSG_CTR){
        Serial.println(" (INVALID MSG)");
      } else if(status_code == WiLP_RETURN_INVALID_CHECKSUM){
        Serial.println(" (CHECKSUM FAILED)");
      } else {
        Serial.println(" (OTHER ERROR)");
      }
      //Serial.print("RSSI: ");
      //Serial.println(rf69.lastRssi(), DEC);
      Serial.println();
    }
    else
    {
      Serial.println("recv failed");
    }
  }
  else
  {
    Serial.println("\nFAULT!!!!\nNo reply, is rf69_server running?\n\n");
  }
}


uint32_t next_fire = 0;

void setup()
{
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  ArduinoOTA.onStart([]() {
    Serial.println("Start updating");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  randomSeed(analogRead(A0));

  yield();

  digitalPinToInterrupt(RFM69_IRQ);


  Serial.println(F("Feather ESP8266 RFM69 TX Datagram Test! (with rotary encoder)"));
  Serial.println();

  if (!rf69.init())
    Serial.println("init failed");
  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM (for low power module)
  // No encryption
  if (!rf69.setFrequency(915.0))
    Serial.println("setFrequency failed");

  // If you are using a high power RF69 eg RFM69HW, you *must* set a Tx power with the
  // ishighpowermodule flag set like this:
  rf69.setTxPower(20, true);
  rf69.setModemConfig(RH_RF69::GFSK_Rb250Fd250);

  // The encryption key has to be the same as the one in the server
  uint8_t key[] = { 0xFF, 0xAE, 0x16, 0x03, 0xE1, 0x10, 0x07, 0x03,
                    0x1F, 0x44, 0xE1, 0x66, 0x65, 0x55, 0xF0, 0xB6
                  };
  rf69.setEncryptionKey(key);

  rf69.setCADTimeout(2);

  EEPROM.begin(4096);

/*
  for(uint16_t idx = 0; idx<4096; idx++){
    EEPROM.write(idx,0);
  }
  EEPROM.commit();
  Serial.println("EEPROM has been erased!");
*/

  handler.initStorage();

  next_fire = millis() + 5000;
}


void loop()
{
  ArduinoOTA.handle();

  if(millis() > next_fire)
  {
    next_fire = millis() + 500;
    sendMessage();
  }
}
