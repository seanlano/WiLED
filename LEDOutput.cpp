/*
* LEDOutput class
* Part of the "wi-led" project, https://github.com/seanlano/wi-led
* A C++ class for controlling a PWM dimmable LED on an Arduino. 
* Copyright (C) 2017 Sean Lanigan
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

#include "LEDOutput.h"


LEDOutput::LEDOutput(int inLEDPin){
	/// Initialise a dimmable LED
	led_pin = inLEDPin;
	analogWrite(led_pin, 0);
	
	#if NUM_DIM_STEPS>1
		float max_log10 = log10(MAX_PWM);
		float steps_log10 = log10(NUM_DIM_STEPS-1);
		float exponent = max_log10/steps_log10;
		float result;
		pwm_dim_levels[0] = 0;
		for(int idx=1; idx<NUM_DIM_STEPS; idx++){
			result = pow(idx,exponent);
			pwm_dim_levels[idx] = int(result);
		}
		pwm_dim_levels[NUM_DIM_STEPS-1] = MAX_PWM;
	#else
		pwm_dim_levels[0] = MAX_PWM;
	#endif
}


// Public methods

void LEDOutput::process(){
	/// Update the LED output
	// First, process any ongoing fade event
	if (__state_fade_inprogress){
		unsigned long millis_now = millis();
		// If we have passed the end time, set the output to the target value
		if (millis_now > __state_fade_end_millis){
			setDimPWMExact(__state_fade_pwm_target);
			// End the fade event
			__state_fade_inprogress = false;
			// Flag that the status callback should be called
			__status_update_needed = true;
		} 
		// Otherwise, calculate what the PWM output should be 
		else {
			// Calculate how many millis we have left to fade for
			float millis_remain = __state_fade_end_millis - millis_now;
			// Calculate which PWM step we should be at
			byte pwm_delta = __state_fade_pwm_target - int(millis_remain * __state_fade_pwmconst);
			// Set the output to that value
			setDimPWMExact(pwm_delta);
		}
	} // End fade processing
	
	// Next, update the PWM output if needed
	// If the power state is "ON", set the output to the latest PWM value
	if (__state_power_on){
		// Check if the output needs updating
		if (__state_pwm != __state_pwm_last){
			analogWrite(led_pin,__state_pwm);
			__state_pwm_last =__state_pwm;
			// If we are not fading, then call the status update callback
			if (!__state_fade_inprogress){
				__status_update_needed = true;
			}
		}
	} 
	// If the power state is "OFF", set the output to 0 but preserve the
	// the value in __state_pwm
	else {
		if (__state_pwm_last != 0){
			analogWrite(led_pin,0);
			__state_pwm_last = 0;
			// If we are not fading, then call the status update callback
			if (!__state_fade_inprogress){
				__status_update_needed = true;
			}
		}
	}
	// Finally, call the status callback if it is needed
	if (__status_update_needed){
		// Call the status callback
		if (__status_callback_exists){
			__status_callback();
		}
		__status_update_needed = false;
	}
}

void LEDOutput::setPowerOn(bool inPower){
	/// Turn the LED on or off. Setting PWM level does not turn the LED on,
	/// it must be turned on explicitly with this function call
	if (inPower){
		__state_power_on = true;
	} 
	else {
		__state_power_on = false;
	}
}

void LEDOutput::setPowerOn(){
	/// Directly set the power to be "on"
	__state_power_on = true;
}

void LEDOutput::setPowerOff(){
	/// Directly set the power to be "off"
	__state_power_on = false;
}

void LEDOutput::setDimStep(int inDimStep){
	/// Set the desired output level, in terms of the dimming step
	// Check the input is sane, i.e. is between 0 and the maximum step
	if (inDimStep > NUM_DIM_STEPS-1){
		__state_dim_level = NUM_DIM_STEPS-1;
	} 
	else if (inDimStep < 0){
		__state_dim_level = 0;
	}
	else {
		__state_dim_level = inDimStep;
	}
	// Set the PWM to the value corresponding to this dim level
	setDimPWM(pwm_dim_levels[__state_dim_level]);
	// Note that setDimPWM() will set __state_dim_level again by
	// re-calculating the closest dim level - even though we just 
	// specified it, but this doesn't really matter
}

void LEDOutput::setDimStepUp(){
	/// Increase the dim level by one, if not already maximum
	// End an in-progress fade, if needed
	setDimFadeStop();
	// Increment the dim level
	if(__state_dim_level < NUM_DIM_STEPS-1){
		__state_dim_level++;
	} 
	else {
		__state_dim_level = NUM_DIM_STEPS-1;
	}
	// Set the PWM to the value corresponding to this dim level
	setDimPWM(pwm_dim_levels[__state_dim_level]);
}

void LEDOutput::setDimStepDown(){
	/// Decrease the dim level by one, if not already zero
	// End an in-progress fade, if needed
	setDimFadeStop();
	// Decrement the dim level
	if (__state_dim_level > 0){
		__state_dim_level--;
	} 
	else {
		__state_dim_level = 0;
	}
	// Set the PWM to the value corresponding to this dim level
	setDimPWM(pwm_dim_levels[__state_dim_level]);
}

void LEDOutput::setDimPercent(int inDimPercent){
	/// Set the desired output level; in terms of a percentage
	byte percent;
	int pwm;
	// Check input is sane, i.e. is a valid percentage 0-100
	if (inDimPercent > 100){
		percent = 100;
	} 
	else if (inDimPercent < 0){
		percent = 0;
	} 
	else {
		percent = inDimPercent;
	}
	// Scale the percentage to the range of the PWM values
	pwm = (percent*MAX_PWM)/100;
	// Set dim level, to the nearest step
	setDimPWM(pwm);
}

void LEDOutput::setDimPWM(int inPWM){
	/// Set the desired output level; in terms of the PWM level (i.e. 0-255)
	/// This will round up to the nearest step
	__sane_pwm = __sane_in_pwm(inPWM);
	int closest_step = __find_closest_step(__sane_pwm);
	// If the default fade duration has been set, start a fade event
	if (__state_fade_default_millis > 0){
		setDimFadeStart(pwm_dim_levels[closest_step], __state_fade_default_millis);
	}
	// Otherwise, just set the value directly
	else {
		setDimPWMExact(pwm_dim_levels[closest_step]);
	}
}

void LEDOutput::setDimPWMExact(int inPWM){
	/// Set the desired output level; in terms of the PWM level (i.e. 0-255)
	/// This function should be the only one used to change the output, 
	/// it will also update the percent and step state values
	// If we have a dimmable LED, set PWM level appropriately
	#if NUM_DIM_STEPS>1
		__state_pwm = __sane_in_pwm(inPWM);
		// If __state_pwm is less than 3, __state_percent would truncate to 0
		if (__state_pwm < 3 && __state_pwm > 0){
			//  Instead, set it to 1%
			__state_percent = 1;
		} 
		else {
			__state_percent = int((100*__state_pwm)/MAX_PWM);
		}
		__state_dim_level = __find_closest_step(__state_pwm);
	// Otherwise, we don't have any dimming so always set to maximum
	#else
		__state_pwm = MAX_PWM;
		__state_percent = 100;
		__state_dim_level = 0;
	#endif
}

void LEDOutput::setDimFadeStart(int inTargetPWM, int inTimeMillis){
	/// Start fading down to the given target PWM level, over a timespan
	/// of the given number of milliseconds
	// Make sure input time is not negative
	if (inTimeMillis < 0){inTimeMillis = abs(inTimeMillis);}
	// Store the target PWM value (after sanitising it)
	__state_fade_pwm_target = __sane_in_pwm(inTargetPWM);
	// Calculate millis time we need to reach the target at
	__state_fade_end_millis = millis() + inTimeMillis;
	// Calculate the rate at which we need to change the PWM output
	__state_fade_pwmconst = (__state_fade_pwm_target - __state_pwm);
	__state_fade_pwmconst = __state_fade_pwmconst / inTimeMillis;
	// Flag that we are in a dimming cycle
	__state_fade_inprogress = true;
}

void LEDOutput::setDimFadeStop(){
	/// Immediately end the fading event, and set to the target value
	// Only cancel the fade if one is in progress
	if (__state_fade_inprogress){
		setDimPWMExact(__state_fade_pwm_target);
		// End the fade event
		__state_fade_inprogress = false;
	}
}

void LEDOutput::setDimDefaultFade(int inTimeMillis){
	/// Set the default fade duration
	__state_fade_default_millis = abs(inTimeMillis);
}

int LEDOutput::getDimPWM(){
	if (__state_power_on){
		return __state_pwm;
	} 
	else {
		return 0;
	}
}

int LEDOutput::getDimPercent(){
	if (__state_power_on){
		return __state_percent;
	} 
	else {
		return 0;
	}
}

int LEDOutput::getDimStep(){
	if (__state_power_on){
		return __state_dim_level;
	} 
	else {
		return 0;
	}
}

unsigned int LEDOutput::getDimDefaultFade(){
	return __state_fade_default_millis;
}

bool LEDOutput::getPowerOn(){
	return __state_power_on;
}

void LEDOutput::setStatusCallback(void (*cb)(void)){
	/// Store the callback function
	__status_callback = cb;
	__status_callback_exists = true;
}


// Private methods
int LEDOutput::__find_closest_step(int inPWM){
	/// Find the nearest PWM step to the given input
	int closest_step = 0;
	__sane_pwm = __sane_in_pwm(inPWM);
	for (int idx=0; idx<NUM_DIM_STEPS; idx++){
		if (__sane_pwm <= pwm_dim_levels[idx]){
			closest_step = idx;
			break;
		}
	}
	return closest_step;
}

int LEDOutput::__sane_in_pwm(int inPWM){
	/// Checking sanity of input PWM value is done often, so it gets a function
	if (inPWM > MAX_PWM){
		__sane_pwm = MAX_PWM;
	} 
	else if (inPWM < 0){
		__sane_pwm = 0;
	}
	else {
		__sane_pwm = inPWM;
	}
	return __sane_pwm;
}
