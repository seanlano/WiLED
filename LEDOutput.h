/*
* LEDOutput class
* Part of the "wi-led" project, https://github.com/seanlano/wi-led
* A C++ class for controlling a PWM dimmable LED on an Arduino. 
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

#ifndef LEDOUTPUT_H
#define LEDOUTPUT_H

#include <Arduino.h>
#include <math.h>

#define MAX_PWM 255		 // The number of PWM levels, usually 255
#define NUM_DIM_STEPS 7 // The number of steps on the dimming scale


class LEDOutput {
	public:
		// Create an LED output controller for the given pin
		LEDOutput(int inLEDPin);
		
		// Run in each cycle of the main loop, to update the LED output
		void process();
		
		// Turn the LED on or off
		void setPowerOn(bool inPower);
		void setPowerOn();  // Directly turn on
		void setPowerOff(); // Directly turn off
		
		// Set the desired output level, in terms of the dimming step
		void setDimStep(int inDimStep);
		
		// Step up or down to the next brightness step
		void setDimStepUp();
		void setDimStepDown();
		
		// Set the desired output level; in terms of a percentage (rounds up to nearest step)
		void setDimPercent(int inDimPercent);
		
		// Set the desired output level; in terms of the PWM level (i.e. 0-255) (rounds up to nearest step)
		void setDimPWM(int inPWM);
		
		// Set the desired output level; in terms of the PWM level (i.e. 0-255) (no rounding)
		void setDimPWMExact(int inPWM);
		
		// Begin fading PWM level over specified duration
		void setDimFadeStart(int inTargetPWM, int inTimeMillis);
		
		// Stop fading, and jump straight to target PWM state
		void setDimFadeStop(); 
		
		// Set the default fade duration, used when changing PWM output
		void setDimDefaultFade(int inTimeMillis);
		
		// Set status update callback. Will be called when LED status changes. 
		void setStatusCallback(void (*cb)(void));
		
		
		// A bunch of get methods to access the internal state variables
		int getDimPWM();
		int getDimPercent();
		int getDimStep();
		unsigned int getDimDefaultFade();
		bool getPowerOn();
		
	private:
		unsigned short led_pin;
		int pwm_dim_levels[NUM_DIM_STEPS];
		unsigned short __state_dim_level = 0;
		unsigned short __state_dim_level_goal = 0;
		bool __state_power_on = false;
		unsigned short __state_percent = 0;
		int __state_pwm = 0;
		int __state_pwm_last = 0;
		int __sane_pwm;
		
		int __state_fade_pwm_target = 0;
		unsigned long __state_fade_end_millis = 0UL;
		unsigned short __state_fade_default_millis = 0;
		float __state_fade_pwmconst = 0.00;
		bool __state_fade_inprogress = false;
		
		bool __status_update_needed = false;
		
		int __find_closest_step(int inPWM);
		int __sane_in_pwm(int inPWM);
		
		void (*__status_callback)(void);
		bool __status_callback_exists = false;
};

#endif
