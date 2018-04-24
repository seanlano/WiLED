# WiLED 

WiLED is a project that aims to create a simple but highly effective control system for home (and commercial) lighting applications. 

The main targets are initially stand-alone lamps, i.e. those that connect to a general purpose power outlet. Future goals also include embedded fixed lighting. 

The target hardware is (Arduino-powered) ESP8266 modules for the end devices, and a Feather M0 coupled to a Raspberry Pi Zero W for the coordinator. 

This repository provides the code for WiLED. See the [WiLED-PCB](https://github.com/seanlano/WiLED-pcb) repository for the circuit schematics. 

## Installation and usage

The code is written for Arduino-powered devices - at the moment, the ESP8266 and the Feather M0. 

*TODO* 

## Roadmap

The overall goal for WiLED is to provide a flexible system for lighting control, powered by MQTT. 

The current system plan is to use an RFM-69 radio to connect the lighting devices together, and then have a coordinator that interfaces the radio link to an MQTT broker. 

### Goals
- Extensible and open system 
  - MQTT interface for integration with other systems 
  - Open source schematics and software 
- Ability to control multiple light outputs with a single switch or dial, through groups 
- Separation of lighting output device and control input device, to allow for flexible reconfiguration 
  - Option to have hardwired controller so lighting is usable even with failure in radio network 
- Tested performance metrics for switching latency 
- Bindings for OpenHAB 

## Contributors and thanks 

The Rotary library for Arduino, by Ben Buxton, has been used and the files `Rotary.h` and `Rotary.cpp` are included in this project. [See here for the original source](https://github.com/brianlow/Rotary). It is GPLv3 licensed. 

Additionally, the wonderful RadioHead library from Mike McCauley and AirSpayce has been used via an inclusion through PlatformIO. [See here for the homepage](http://www.airspayce.com/mikem/arduino/RadioHead/index.html). It is GPLv2 licensed (not clear if this is GPLv2 _only_ or GPLv2+). 

For CRC16 checksum calculation, a function based on work by Vincenzo Mennella has been created. [See here for the original](https://github.com/vinmenn/Crc16/blob/aed958375acfd6edfcddbfcb3a42a861c77bb697/Crc16.h). His work is [MIT licensed](https://github.com/vinmenn/Crc16/blob/aed958375acfd6edfcddbfcb3a42a861c77bb697/LICENSE). The new version,  `CRC16.cpp` and `CRC16.h`, as part of WiLED, is licensed under the GPL like the rest of this project. 

The Switch library from Dean Blackketter has also been used and included in this project. [See here for the original source](https://github.com/blackketter/Switch). It is GPLv3 licensed. Some changes have been made to `Switch.cpp` and `Switch.h` compared to the original, in particular to allow the class to read from an analogue input and compare the reading to a threshold to determine switch state. 

For unit tests, the [Google Test framework](https://github.com/google/googletest) is used. Necessary files are directly included in this repository, so that it is easy to compile and run unit tests without needing to separately download the Google Test source. Google Test is licensed under the [New BSD License](https://github.com/google/googletest/blob/master/googletest/LICENSE). 

## License

This project is released under the terms of the GNU GPL version 3 (or later). Please see [LICENSE](LICENSE) for details. 
