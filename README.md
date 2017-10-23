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
- Tested performance metrics for switching latency tm
- Bindings for OpenHAB 

## Contributors and thanks 

The Rotary library for Arduino, by Ben Buxton, has been used and the files `Rotary.h` and `Rotary.cpp` are included in this project. [See here for the original source](https://github.com/brianlow/Rotary). It is GPLv3 licensed. 

## License

This project is released under the terms of the GNU GPL version 3 (or later). Please see [LICENSE](LICENSE) for details. 
