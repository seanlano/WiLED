/*
Switch
Copyright (C) 2012  Albert van Dalen http://www.avdweb.nl
This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License
as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License at http://www.gnu.org/licenses .

This version modified by Sean Lanigan, for use in the WiLED project.
See Git revision control history for changes made from the original.
In partiular, edits have been made to support reading from an analogue input.


*/

#ifndef SWITCH_H
#define SWITCH_H

#include <hal.h>

// Uncomment this to use analogue mode.
// 1 is "normally high" and 0 is "normally low"
#define ANALOG_MODE 1
#define ANALOG_THRESH 512 // Set this to be the threshold analogue read value

typedef void (*switchCallback_t)(void*);

class Switch
{
public:
  Switch(hal_Switch *inSwitch, bool polarity=LOW, int debounceDelay=50, int longPressDelay=400, int doubleClickDelay=250);
  Switch(hal_Switch *inSwitch, hal_Millis *inMillis, bool polarity=LOW, int debounceDelay=50, int longPressDelay=400, int doubleClickDelay=250);
  bool poll(); // Returns 1 if switched
  bool switched(); // will be refreshed by poll()
  bool on();
  bool pushed(); // will be refreshed by poll()
  bool released(); // will be refreshed by poll()
  bool longPress(); // will be refreshed by poll()
  bool longPressLatch();
  bool doubleClick(); // will be refreshed by poll()

  unsigned long _switchedTime, pushedTime;

  // Set methods for event callbacks
  void setPushedCallback(switchCallback_t cb, void* param = nullptr);
  void setReleasedCallback(switchCallback_t cb, void* param = nullptr);
  void setLongPressCallback(switchCallback_t cb, void* param = nullptr);
  void setDoubleClickCallback(switchCallback_t cb, void* param = nullptr);

protected:
  hal_Switch* _switch;
  hal_Millis* _millis;
  const uint16_t debounceDelay, longPressDelay, doubleClickDelay;
  const bool polarity;
  bool level, _switched, _longPress, _longPressLatch, _doubleClick;

  // Internal function for using either analogue or digital mode
  bool read_input();

  // Event callbacks
  switchCallback_t _pushedCallback = nullptr;
  switchCallback_t _releasedCallback = nullptr;
  switchCallback_t _longPressCallback = nullptr;
  switchCallback_t _doubleClickCallback = nullptr;

  void* _pushedCallbackParam = nullptr;
  void* _releasedCallbackParam = nullptr;
  void* _longPressCallbackParam = nullptr;
  void* _doubleClickCallbackParam = nullptr;

};

#endif