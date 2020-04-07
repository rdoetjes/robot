#include "rangesensor.h"

RangeSensor::RangeSensor(int pTrig, int pEcho) {
  this->pTrig = pTrig;
  this->pEcho = pEcho;
 
  pinMode(pTrig, OUTPUT);
  pinMode(pEcho, INPUT);
}

float RangeSensor::measure(){
  std::cout << "echo status " << digitalRead(pEcho) << std::endl;
  //send 10 micro second ping
  digitalWrite(pTrig, HIGH);
  delayMicroseconds(10);
  digitalWrite(pTrig, LOW);
  std::cout << "ping send" << std::endl;

  unsigned int echoStart = millis();
  std::cout << "waiting for echo to go low" << std::endl;
  while(digitalRead(pEcho) == LOW && millis()-echoStart < 1000) { }
  std::cout << millis()-echoStart << std::endl;
  if (millis()-echoStart < 1000) {
      // Mark start
      unsigned int start = micros();
      std::cout << "waiting for echo to go high" << std::endl;
      while(digitalRead(pEcho) == HIGH) { }
      // Mark end
      unsigned int end = micros();
      unsigned int delta = end-start;

      return 17014 * delta; 
  }

  return -1;
}

RangeSensor::~RangeSensor(){
  this->pTrig = this->pEcho = -1;
}
