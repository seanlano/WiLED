/*
* RunMode class
* Part of the "wi-led" project, https://github.com/seanlano/wi-led
* A class for controlling run mode or settings mode. 
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

#ifndef RUNMODE_H 
#define RUNMODE_H 

#include <Arduino.h>

#define NUM_SETTING_MODES 3  
#define INCLUDE_MODE_ZERO true 


class IndicatorOutput 
{
	public:
		// Accept an instantiation wihtout arguments, although it won't do anything 
		IndicatorOutput();
		// Instantiate the IndicatorOutput class with a given LED pin 
		IndicatorOutput(uint8_t inLEDPin);
		
		// Update the output 
		void update(); 
		
		// Reset timers
		void reset();
		
		// Set an exact PWM value
		void setExact(uint16_t inPWM);
		
		// Set the mode number
		void setBlinkMode(uint8_t inMode);
		
	private:
		// External value variables 
		uint8_t __led_pin; 
		uint8_t __blink_mode = 0; 
		
		// Internal tracking variables 
		uint8_t __output_step = 0; 
		uint16_t __pwm_low = 2; 
		uint16_t __pwm_high = 255; 
		uint32_t __output_step_next_millis = 0; 
		uint16_t __output_step_spacing_millis = 150; 
};

class RunMode 
{
	public:
		// Instantiate the RunMode class with a given indicator LED pin 
		RunMode(uint8_t inLEDPin);
		
		// Run in each cycle of the main loop 
		void process(); 
		
		// Set the run mode 
		void setModeSettings(); 
		void setModeNormal(); 
		
		// Select button press 
		void select(); 
		// Next button press / dial turn 
		void next(); 
		// Previous button press / dial turn 
		void prev(); 
	
		// Define various get functions
		bool getModeNormal(); 
		
		IndicatorOutput indicator; 
		
	private:
		// Run mode
		bool __mode_normal = true;  
		uint8_t __setting_mode = 0; 
};



#endif 
