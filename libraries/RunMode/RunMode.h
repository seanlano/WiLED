/*
* RunMode class
* Part of the "WiLED" project, https://github.com/seanlano/WiLED
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

#include <hal.h>

#define NUM_SETTING_MODES 3  
#define INCLUDE_MODE_ZERO true 


class IndicatorOutput 
{
	public:
		// Instantiate the IndicatorOutput class with a given LED pin 
		IndicatorOutput(hal_LED *inLED);
		
		// Update the output 
		void update(); 
		
		// Reset timers
		void reset();
		
		// Set an exact PWM value
		void setExact(uint16_t inPWM);
		
		void setNormal();
		void setNormal(uint16_t inPWM);
		// Set the blink mode number
		void setBlink();
		void setBlink(uint8_t inMode);
		
		// Set double-flash mode 
		void setDoubleFlash();
		
	protected:
		// External value variables 
		hal_LED* _led; 
		uint8_t _blink_mode = 0; 
		uint8_t _output_mode = 0;
		
	private: 
		// Internal tracking variables 
		uint8_t _output_step = 0; 
		uint16_t _pwm_low = 2; 
		uint16_t _pwm_high = 255; 
		uint16_t _pwm_normal = 0;
		uint32_t _output_step_next_millis = 0; 
		uint16_t _output_step_spacing_millis = 200; 
};

class RunMode : public IndicatorOutput 
{
	public:
		// Instantiate the RunMode class with a given indicator LED pin 
		RunMode(hal_LED *inLED);
		
		// Run in each cycle of the main loop 
		void update(); 
		
		// Select button press 
		void select(); 
		// Next button press / dial turn 
		void next(); 
		// Previous button press / dial turn 
		void prev(); 
	
		// Define various get functions
		bool getModeNormal(); 
		
	protected:
		
};



#endif 
