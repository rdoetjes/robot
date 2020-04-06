#ifndef MOTOR_H
#define MOTOR_H

#include <wiringPi.h>

class Motor {

  public:
    Motor(int p1, int p2);
    void forward();
    void reverse();
    void stop();

  private:
    int p1;
    int p2;
    virtual ~Motor();
};

#endif
