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
