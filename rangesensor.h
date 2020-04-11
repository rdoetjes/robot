/*
 RangeSensor class (Raymond Doetjes 2020)
 Implements the low lever workings of an HC-SR04.
 It uses the WiringPi library, check that for more details.

 Connecting to Pi: 
    VCC to 5V line
    Trigger to an arbitraty GPIO pin (3.3V is considered TTL high)
    Echo requires to have a voltage divider, I used 1K and 1.5K    
    ECHO---[R1]--gpio--[R2]---GND
 Constructor takes the GPIO nummber for Trigger (pTrig) and GPIO for Echo pin
 Simply call the measure method 

 example:
 RangeSensor s=new RangeSensor(7, 14); //trigger is on GPIO7, echo is on GPIO15
 cout << s->measure() << endl;

 compile:
 g++ main.cpp -o test -lwiringpi -lpthread
*/

#ifndef RANGESENSOR_H
#define RANGESENSOR_H

#include <wiringPi.h>
#include <iostream>

class RangeSensor {

  public:
    RangeSensor(int pTrig, int pEcho);
    float measure();
    virtual ~RangeSensor();

  private:
    int pTrig;
    int pEcho;

};

#endif
