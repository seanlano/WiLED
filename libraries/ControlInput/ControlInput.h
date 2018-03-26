/*
* ControlInput.h
* Provides the ControlInput class, a high level object that polls 
* input events and dispatches WiLEDProto messages 
* 
* Part of the "WiLED" project, https://github.com/seanlano/WiLED
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

#ifndef CONTROLINPUT_H
#define CONTROLINPUT_H

#include <hal.h>
#include <math.h>

#define MAX_PWM 1024		 // The number of PWM levels, usually 255
#define NUM_DIM_STEPS 7 // The number of steps on the dimming scale


class ControlInput {
	public:
		// Create an LED output controller for the given pin
		ControlInput(hal_LED *inLED);

		// Run in each cycle of the main loop, to update the LED output
		void process();

		// Set the desired output level, in terms of the dimming step
		void setDimStep(uint8_t inDimStep);

		// Step up or down to the next brightness step
		void setDimStepUp();
		void setDimStepDown();

		// Set the desired output level; in terms of a percentage (rounds up to nearest step)
		void setDimPercent(uint8_t inDimPercent);

		// Set the desired output level; in terms of the PWM level (i.e. 0-255) (rounds up to nearest step)
		void setDimPWM(uint16_t inPWM);

		// Set the desired output level; in terms of the PWM level (i.e. 0-255) (no rounding)
		void setDimPWMExact(uint16_t inPWM);

		// Begin fading PWM level over specified duration
		void setDimFadeStart(uint16_t inTargetPWM, uint16_t inTimeMillis);

		// Set the LED to turn off after a delay
		void setAutoOffTimer(uint32_t inTimeMillis);

		// Stop fading, and jump straight to target PWM state
		void setDimFadeStop();

		// Set the default fade duration, used when changing PWM output
		void setDimDefaultFade(uint8_t inTimeMillis);

		// Set a lockout time to enforce a delay before either maximum step or zero step
		void setDimStepLockout(uint8_t inTimeMillis);

		// Set status update callback. Will be called when LED status changes.
		void setStatusCallback(void (*cb)(void));


		// A bunch of get methods to access the internal state variables
		uint16_t getDimPWM();
		uint8_t getDimPercent();
		uint8_t getDimStep();
		uint8_t getDimDefaultFade();
		bool getPowerOn();

	private:
		hal_LED* led;
		uint16_t pwm_dim_levels[NUM_DIM_STEPS];
		uint8_t _state_dim_level = 0;
		uint8_t _state_dim_level_goal = 0;
		bool _state_power_on = false;
		uint8_t _state_percent = 0;
		int16_t _state_pwm = 0;
		int16_t _state_pwm_last = 0;
		int16_t _sane_pwm;

		int16_t _state_fade_pwm_target = 0;
		uint32_t _state_fade_end_millis = 0UL;
		uint8_t _state_fade_default_millis = 0;
		uint8_t _state_step_lockout_millis = 0;
		uint32_t _state_lockout_end_millis = 0UL;
		float _state_fade_pwmconst = 0.00;
		bool _state_fade_inprogress = false;
		bool _state_autooff = false;
		uint32_t _state_autooff_millis = 0UL;

		bool _status_update_needed = false;

		uint8_t _find_closest_step(uint16_t inPWM);
		uint16_t _sane_in_pwm(uint16_t inPWM);
		uint32_t _millis_now = 0;

		void (*_status_callback)(void) = 0;
};

#endif 