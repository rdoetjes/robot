#ifndef MOTOR_H
#define MOTOR_H

#include <wiringPi.h>
#include <softPwm.h>

class Motor {

  public:
    Motor(int p1, int p2);
    void forward(int pwm);
    void reverse(int pwm);
    void stop();
    virtual ~Motor();

  private:
    int p1;
    int p2;
};

#endif
