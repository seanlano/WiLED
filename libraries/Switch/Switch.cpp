/*
Switch
Copyright (C) 2012  Albert van Dalen http://www.avdweb.nl
This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License
as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License at http://www.gnu.org/licenses .

Version 20-4-2013
_debounceDelay=50
Version 22-5-2013
Added longPress, doubleClick

This version modified by Sean Lanigan, for use in the WiLED project.
See Git revision control history for changes made from the original.
In partiular, edits have been made to support reading from an analogue input.


                        _______________________      _false
                       |                       |   || |
 input                 |                       || ||| |
                  _____|                       ||_||| ||____________

 poll                   ^                        ^   ^          ^
 switchedTime           ^                        ^
                         <---------100ms--------><--->
 debounceDelay           <----50ms---->          <----50ms---->
 switched               1                        1   0          0
 newlevel               1                        0   1          0
                         ________________________
 level             _____|                        |___________________

.......................................................................
                            _______________________             _______
                           |                       |           |
 input                     |                       |           |
                   ________|                       |___________|

 longPressDelay             <----------->

 doubleClickDelay           <-------------------------------------->
                                          _________
 _longPressLatch   ______________________|         |_________________
                                          _
 _longPress        ______________________| |__________________________
                                                                _
_doubleClick       ____________________________________________| |____

*/

#include "Switch.h"

Switch::Switch(hal_Switch *inSwitch, bool polarity, int debounceDelay, int longPressDelay, int doubleClickDelay):
polarity(polarity), debounceDelay(debounceDelay), longPressDelay(longPressDelay), doubleClickDelay(doubleClickDelay)
{
  _switch = inSwitch;
  _millis = new hal_Millis;
  _switchedTime = _millis->millis();
  level = read_input();
}

Switch::Switch(hal_Switch *inSwitch, hal_Millis *inMillis, bool polarity, int debounceDelay, int longPressDelay, int doubleClickDelay):
polarity(polarity), debounceDelay(debounceDelay), longPressDelay(longPressDelay), doubleClickDelay(doubleClickDelay)
{
  _switch = inSwitch;
  _millis = inMillis;
  _switchedTime = _millis->millis();
  level = read_input();
}

bool Switch::read_input()
{
  // Use the hal_Switch function to get the input status
  return _switch->getPin();
}

bool Switch::poll()
{ 
  _longPress = _doubleClick = false;
  bool newlevel = read_input();

  if(!_longPressLatch)
  { _longPress = on() && (unsigned((_millis->millis()) - pushedTime) > longPressDelay); // true just one time between polls
    _longPressLatch = _longPress; // will be reset at next switch
  }
  if(_longPressCallback && longPress())
  { _longPressCallback(_longPressCallbackParam);
  }

  if((newlevel != level) & (unsigned(_millis->millis()) - _switchedTime >= debounceDelay))
  { _switchedTime = _millis->millis();
    level = newlevel;
    _switched = 1;
    _longPressLatch = false;

    if(pushed())
    { _doubleClick = (long)(_millis->millis() - pushedTime) < doubleClickDelay;
      pushedTime = _millis->millis();
    }

    if(_pushedCallback && pushed())
    { _pushedCallback(_pushedCallbackParam);
	}
	else if(_releasedCallback && released())
	{ _releasedCallback(_releasedCallbackParam);
	}

    if(_doubleClickCallback && doubleClick())
    { _doubleClickCallback(_doubleClickCallbackParam);
	}
    return _switched;
  }
  return _switched = 0;
}

bool Switch::switched()
{ 
  return _switched;
}

bool Switch::on()
{ 
  return !(level^polarity);
}

bool Switch::pushed()
{ 
  return _switched && !(level^polarity);
}

bool Switch::released()
{ 
  return _switched && (level^polarity);
}

bool Switch::longPress()
{ 
  return _longPress;
}

bool Switch::doubleClick()
{ 
  return _doubleClick;
}

bool Switch::longPressLatch()
{ 
  return _longPressLatch;
}

void Switch::setPushedCallback(switchCallback_t cb, void* param)
{ /// Store the "pushed" callback function
  _pushedCallback = cb;
  _pushedCallbackParam = param;
}

void Switch::setReleasedCallback(switchCallback_t cb, void* param)
{ /// Store the "released" callback function
  _releasedCallback = cb;
  _releasedCallbackParam = param;
}

void Switch::setLongPressCallback(switchCallback_t cb, void* param)
{ /// Store the "long press" callback function
  _longPressCallback = cb;
  _longPressCallbackParam = param;
}

void Switch::setDoubleClickCallback(switchCallback_t cb, void* param)
{ /// Store the "double click" callback function
  _doubleClickCallback = cb;
  _doubleClickCallbackParam = param;
}
