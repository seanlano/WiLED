/* WiLED_esp8266-client-eavesdropper.ino
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


#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#include <string.h>


#include <SPI.h>
#include <RH_RF69.h>

#include "src/WiLEDProto.h"


#define RFM69_CS      16
#define RFM69_IRQ     15

#define ROTARY_A 0
#define ROTARY_B 2 


WiLEDProto handler(0x1000);

const char* ssid = "SSID";
const char* password = "PASS";


// Singleton instance of the radio driver
RH_RF69 rf69(RFM69_CS, RFM69_IRQ);


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

  yield();
  
  digitalPinToInterrupt(RFM69_IRQ);

  
  Serial.println(F("Feather ESP8266 RFM69 RX eavesdropper test!"));
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
  
}

uint32_t last_message = 0;
uint32_t this_message = 0;

void loop()
{
  ArduinoOTA.handle();

  // Listen and report if there is a packet available, otherwise keep looping
  if (rf69.waitAvailableTimeout(10))
  {
    uint8_t buf[RH_RF69_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    // Should be a message for us now   
    if (rf69.recv(buf, &len))
    {
      this_message = millis();
      Serial.print("got message: ");
      Serial.println((char*)buf);

      Serial.print("HEX: ");
      for(int idx=0; idx<20; idx++){
        Serial.print(buf[idx], HEX);
        Serial.print(" ");
      }
      Serial.println(". ");

      uint8_t status_code = handler.processMessage(buf);
      Serial.print("Message analysis. Return code:");
      Serial.println(status_code, DEC);
      Serial.print("  Source device: ");
      Serial.println(handler.getLastReceivedSource(), HEX);
      Serial.print("  Destination device: ");
      Serial.println(handler.getLastReceivedDestination(), HEX);
      Serial.print("  Message type: ");
      Serial.println(handler.getLastReceivedType(), HEX);
      Serial.print("  Message counter: ");
      Serial.println(handler.getLastReceivedMessageCounter(), HEX);
      
      //Serial.print("RSSI: ");
      //Serial.println(rf69.lastRssi(), DEC);
      //Serial.print("Millis delta with last message: ");
      //Serial.println(this_message - last_message);
      Serial.println();
      last_message = this_message;
    }
    else
    {
      Serial.println("recv failed");
    }
  }
}
