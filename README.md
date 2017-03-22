# wi-led
Arduino and ESP8266 powered controller for a dimmable LED, controlled over Wi-Fi using MQTT. 

This is the Arduino code. 

## Installation and usage

The code is written for an ATmega328 based Arduino, running at a base clock of 8 MHz - i.e. the 3.3 V versions of the [Arduino Pro Mini](https://www.arduino.cc/en/Main/ArduinoBoardProMini) and similar. It should also work on the 5 V / 16 MHz versions, although level shifting will then be needed on the serial interface with the ESP8266. 

*TODO* 

## Contributors and thanks 

The Rotary library for Arduino, by Ben Buxton, has been used and the files `Rotary.h` and `Rotary.cpp` are included in this project. [See here for the original source](https://github.com/brianlow/Rotary). It is GPLv3 licensed. 

## License

This project is released under the terms of the GNU GPL version 3 (or later). Please see [LICENSE](LICENSE) for details. 
