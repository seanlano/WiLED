# LEDOutput class

The LEDOutput class provides various useful ways to manage an attached LED driver. 

## Dimming Steps 

LEDOutput provides a customisable number of dimming "steps", by default it has six brightness levels which follow an exponential function. 
This provides a more even perception of brightness change between each step than a linear function, for example the eye will perceive a much bigger difference between 1/255 and 10/255 than it would between 245/255 and 255/255. Hence the difference between the lower steps in terms of the PWM output is much less than the difference between the higher steps. 

## Features

* Discrete dimming steps, using an exponential function to determine the PWM level for each step 
* Support for smoothly fading to a new PWM output level
* Support for a status callback, called when a fade is complete 

## Usage 

The LEDOutput class must be declared with an Arduino output pin number. The pin must be PWM compatible. 
For example, declare the class like this:
```C++
// Initialise a dimmable LED on pin 11
LEDOutput led1(11); 
```
In order to provide features such as fading and status callbacks, LEDOutput needs to be polled often. You must call `process()` in the Arduino main loop, like so: 
```C++
void loop() {
  led1.process();
}
```

## Set methods

### `process()`

Must be called frequently in the main loop to update the output PWM value. 

### `setPowerOn()` and `setPowerOff()`

Sets the output on or off. Can be called without an argument, or as `setPowerOn(true)` or `setPowerOn(false)`. 

### `setDimStep(inDimStep)`

Set the dimming level to the specified step (i.e. 1-6). 

### `setDimStepUp()` and `setDimStepDown()`

Change to the next higher or lower dimming step, respectively. 
If `setDimStepLockout` has been called with a non-zero value, `setDimStepUp` will enforce a lockout time before allowing changing to the highest dimming step; and conversely `setDimStepUp` will enforce a lockout time before changing to zero output. This means that a fast turn of the rotary encoder won't turn the LED all the way on or off in one go, there needs to be a short pause. 

### `setDimPercent(inDimPercent)`

Set the LED output to the specified percentage, i.e. 0-100%. Will not round to the nearest dimming step. 

### `setDimPWM(inPWM)`

Set the LED output to the specified PWM, i.e. 0-255 (if using 8 bit PWM). Will round up to the nearest dimming step, and will fade if `setDimDefaultFade` has been set to a non-zero value. 

### `setDimPWMExact(inPWM)`

Set the LED output to the specified PWM, i.e. 0-255 (if using 8 bit PWM). Similar to the above but will not round to the nearest dimming step and does not fade. 

### `setDimFadeStart(inTargetPWM, inTimeMillis)`

Start fading the LED output to the specified PWM level, over the specifed number of milliseconds. 

### `setDimFadeStop()`

Immediately stop a fade event, and set to the target fade value. 

### `setDimDefaultFade(inTimeMillis)`

If set to a non-zero value, any call to change the LED output will fade for this duration. NOTE: value must be 0-255 milliseconds. 

### `setDimStepLockout(inTimeMillis)`

If set to a non-zero value, `setDimStepUp` and `setDimStepDown` will use this lockout time when attempting to move to the highest or the zero step. NOTE: value must be 0-255 milliseconds. 

### `setStatusCallback(void (*cb)(void))`

Set a status callback, to be called when the output value has changed and any fading is complete. 

For example, define a function to be called:
```C++
void LEDStatusUpdate(){
  // Define a callback to use when the LED output changes
  // Do some things here
}
```
and then in the setup section configure the callback:
```C++
void setup() {
  Serial.begin(57600);
...
  // Attach LED status callback
  led1.setStatusCallback(&LEDStatusUpdate);
...
}
```

## Get methods

### `getDimPWM()`

Returns the current PWM output value. 

### `getDimPercent();

Returns the current output value as a percentage of the maximum PWM value. 

### `getDimStep();

Returns the current dimming step. 

### `getDimDefaultFade();

Retruns the current default dimming fade time. 

### `getPowerOn();

Returns the current power state, as `true` or `false`.

