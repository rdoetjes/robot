/*
  RangeSensor class
  Implements the low lever workings of an HC-SR04.
  Connecting to Pi:
     VCC to 5V line
     Trigger to an arbitraty GPIO pin (3.3V is considered TTL high)
     Echo requires to have a voltage divider, I used 1K and 1.5K 
     ECHO---[R1]--gpio--[R2]---GND
  Constructor takes the GPIO nummber for Trigger (pTrig) and GPIO for Echo pin
  Simply call the measure method 
 
 example:
 RangeSensor s=new RangeSensor(7, 14); //trigger on GPIO 7, echo on GPIO 14
 cout << s->measure() << endl;

 compile:
 g++ main.cpp -o test -lwiringpi -lpthread
*/


#include "rangesensor.h"
#include <future>

RangeSensor::RangeSensor(int pTrig, int pEcho) {
  this->pTrig = pTrig;
  this->pEcho = pEcho;
 
  pinMode(pTrig, OUTPUT);
  pinMode(pEcho, INPUT);

  digitalWrite(pTrig, LOW);
}

float RangeSensor::measure(){
  long ping = 0;
  long pong = 0;
  long  microsold = 0;
  unsigned int timeout = 50000; 

  //send 10 micro second ping
  digitalWrite(pTrig, HIGH);
  delayMicroseconds(10);
  digitalWrite(pTrig, LOW);

  microsold = micros();
  // Wait for ping response, or timeout.
  while (digitalRead(pEcho) == LOW && (micros()-microsold < timeout)) { }
	
  // Cancel on timeout.
  if (micros()-microsold  > timeout) return -1;
	
  ping = micros();
	
  // Wait for pong response, or timeout.
  while (digitalRead(pEcho) == HIGH && (micros() - ping < timeout)) { }

  // Cancel on timeout.
  if (micros()-ping > timeout) return -1;

  pong = micros();

  return  (float) (pong - ping) * 0.017150;

}

RangeSensor::~RangeSensor(){
  this->pTrig = this->pEcho = -1;
}
