#include "motor.h"

Motor::Motor(int p1, int p2){
  this->p1 = p1;
  this->p2 = p2;
  pinMode(p1, OUTPUT);
  pinMode(p2, OUTPUT);
 
  softPwmCreate (p1, 100, 100);
  softPwmCreate (p2, 100, 100);
}

void Motor::forward(int pwm){
   softPwmWrite (p1, pwm);
   softPwmWrite (p2, pwm);
   digitalWrite(p1, HIGH);
   digitalWrite(p2, LOW);
}

void Motor::reverse(int pwm){
   softPwmWrite (p1, pwm);
   softPwmWrite (p2, pwm);
   digitalWrite(p1, LOW);
   digitalWrite(p2, HIGH);
}

void Motor::stop(){
   digitalWrite(p1, LOW);
   digitalWrite(p2, LOW);
}

Motor::~Motor(){
  this->stop();
  p1=p2=-1;
}
