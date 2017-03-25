# LEDOutput class

The LEDOutput class provides various useful ways to manage an attached LED driver. 


## Usage 

The LEDOutput class must be declared with an Arduino output pin number. The pin must be PWM compatible. 
For example, declare the class like this:

    // Initialise a dimmable LED on pin 11
    LEDOutput led1(11); 

In order to provide features such as fading and status callbacks, LEDOutput needs to be polled often. You must call `process()` in the Arduino main loop, like so: 

    void loop() {
      led1.process();
    }


