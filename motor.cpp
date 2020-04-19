/*
 Motor class 
 This is a lowlevel class to drive individual motors.
 This class tends to be wrapped in a function or a more highlevel class

 Connecting is done via an H-Bridge:      
    VCC on HBridge to 5V line on dedicated battery
    GND on HBridge to GND shared with GND on Pi
    A-1A is connected to an arbitrary GPIO
    A-2A is connected to an arbitrary GPIO

 Constructor takes the GPIO number for p1 (gpio A-1A) and p2 (gpio A-@A)
 Bare in mind, that you can swap these numbers, when the motor turns backwards
 when you want it to turn forwards.

 The WiringPi SoftPwm is used to lower the speed by using PWM.

 call methods:
 foreward(100); to run at full speed forwards
 reverse(100); to run at full speed backwards
 
 example:
 Motor m=new Motor(11, 12);
 m->forward(90); //90% of the speed
 m->reverse(90); //90% of the speed
 m->stop(); //stop running the motor
*/

#include "motor.h"

Motor::Motor(int p1, int p2){
  this->p1 = p1;
  this->p2 = p2;
  pwmSetMode(PWM_MODE_BAL);
  pinMode(p1, PWM_OUTPUT);
  pinMode(p2, OUTPUT);
}

void Motor::forward(int pwm){
   pwmWrite(p1, pwm);
   digitalWrite(p2, LOW);
}

void Motor::reverse(int pwm){
   pwmWrite(p1, pwm);
   digitalWrite(p2, HIGH);
}

void Motor::stop(){
   pwmWrite(p1, 0);
   digitalWrite(p2, LOW);
}

Motor::~Motor(){
  this->stop();
  pinMode(p1, OUTPUT);
  pinMode(p2, OUTPUT);
  digitalWrite(p1, LOW);
  digitalWrite(p2, LOW);

  p1=p2=-1;
}
