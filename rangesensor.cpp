#include "rangesensor.h"

RangeSensor::RangeSensor(int pTrig, int pEcho) {
  this->pTrig = pTrig;
  this->pEcho = pEcho;
 
  pinMode(pTrig, OUTPUT);
  pinMode(pEcho, INPUT);
}

float RangeSensor::measure(){
  long ping = 0;
  long pong = 0;
  unsigned int timeout = 50000; 
  //send 10 micro second ping
  digitalWrite(pTrig, HIGH);
  delayMicroseconds(10);
  digitalWrite(pTrig, LOW);

  // Wait for ping response, or timeout.
  while (digitalRead(pEcho) == LOW && micros() < timeout) { }
	
  // Cancel on timeout.
  if (micros() > timeout) return -1;
	
  ping = micros();
	
  // Wait for pong response, or timeout.
  while (digitalRead(pEcho) == HIGH && micros() < timeout) { }
  if (micros() > timeout) return -1;
  pong = micros();
	
  return  (float) (pong - ping) * 0.017150;

}

RangeSensor::~RangeSensor(){
  this->pTrig = this->pEcho = -1;
}
