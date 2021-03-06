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

#include "RunMode.h"


IndicatorOutput::IndicatorOutput(uint8_t inLEDPin)
{
	/// Initialise the LED indicator output 
	__led_pin = inLEDPin;
}

void IndicatorOutput::update()
{
	/// Update the PWM output
	uint32_t millis_now = millis();
	// Check if we should move to the next output_step
	switch(__output_mode){
		// Normal mode
		case 0:
			break;
		// Blink mode 
		case 1:
		{
			if(millis_now > __output_step_next_millis){
				if(__blink_mode == 0){
					// Special output for mode 0
					// Check if we need to loop around to step 0
					if(__output_step > 6){
						__output_step = 0;
					}
					// High for step 0-4
					if(__output_step < 5){
						setExact(__pwm_high);
					// Low for step 5-6
					} else {
						setExact(__pwm_low);
					}
				} else {
					// Not 0, use algorithm 
					// Check if we need to loop around to step 0 
					if(__output_step > (__blink_mode * 2 + 2)){
						__output_step = 0;
					}
					// If output_step is greater than (setting_mode*2-1), set output low 
					if(__output_step > (__blink_mode * 2 - 1)){
						setExact(__pwm_low);
					} else if(__output_step % 2){
						// If step is odd, set output low
						setExact(__pwm_low);
					} else {
						// If step is even, set output high 
						setExact(__pwm_high);
					}
				}
				// Set the next update time 
				__output_step_next_millis = millis_now + __output_step_spacing_millis; 
				__output_step++;
			}
			break;
		}
		// Double-flash mode 
		case 2:
		{
			if(millis_now > __output_step_next_millis){
				// High for step 0,2
				if((__output_step == 0) || (__output_step == 2)){
					setExact(__pwm_high);
				// Off for step 1,3
				} else {
					setExact(0);
				}
				// Check if we need to loop around to step 0
				if(__output_step > 3){
					__output_step = 0;
					// End the blink mode, return to normal 
					setNormal();
				}
				// Set the next update time 
				__output_step_next_millis = millis_now + __output_step_spacing_millis; 
				__output_step++;
			}
			break;
		}
	}
}

void IndicatorOutput::setExact(uint16_t inPWM)
{
	/// Write out an exact PWM value
	analogWrite(__led_pin, inPWM);
}

void IndicatorOutput::setNormal()
{
	/// Set output mode to "normal"
	setNormal(__pwm_normal);
}

void IndicatorOutput::setNormal(uint16_t inPWM)
{
	/// Set output mode to "normal", with a given PWM value 
	// Update the normal PWM value 
	__pwm_normal = inPWM;
	__output_mode = 0;
	setExact(__pwm_normal);
}

void IndicatorOutput::setBlink()
{
	/// Set the output mode to "blink"
	__output_mode = 1; 
	reset();
}

void IndicatorOutput::setBlink(uint8_t inMode)
{
	/// Set the output mode to "blink"
	__blink_mode = inMode;
	setBlink();
}

void IndicatorOutput::setDoubleFlash()
{
	/// Set the output mode to "double-flash"
	__output_mode = 2;
	reset();
}

void IndicatorOutput::reset()
{
	/// Set timing variables to zero 
	__output_step_next_millis = millis() + __output_step_spacing_millis; 
	__output_step = 0;
	setExact(__pwm_low);
}



RunMode::RunMode(uint8_t inLEDPin) : IndicatorOutput(inLEDPin)
{
	/// Initialise the mode controller 
}

void RunMode::update()
{
	/// Update the system state 
	IndicatorOutput::update();
}

void RunMode::next()
{
	/// Process the "next" menu item. if on blink mode 
	if(IndicatorOutput::__output_mode == 1){
		// In settings mode, move to next setting number
		if(IndicatorOutput::__blink_mode < NUM_SETTING_MODES){
			IndicatorOutput::__blink_mode++;
		}
		IndicatorOutput::setBlink();
	}
}

void RunMode::prev()
{
	/// Process the "previous" menu item. if on blink mode 
	if(IndicatorOutput::__output_mode == 1){
		// In settings mode, move to previous setting number
		if((INCLUDE_MODE_ZERO && (IndicatorOutput::__blink_mode > 0))
			|| (!INCLUDE_MODE_ZERO && (IndicatorOutput::__blink_mode > 1))){
			IndicatorOutput::__blink_mode--;
		}
		IndicatorOutput::setBlink();
	}
}

void RunMode::select()
{
	/// Process a "select" of the current menu item 
	IndicatorOutput::setNormal();
}

bool RunMode::getModeNormal()
{
	/// Return true if in normal run mode 
	if(IndicatorOutput::__output_mode == 0){
		return true;
	} else {
		return false;
	}
}

