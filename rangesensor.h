#ifndef RANGESENSOR_H
#define RANGESENSOR_H

#include <wiringPi.h>
#include <iostream>

class RangeSensor {

  public:
    RangeSensor(int pTrig, int pEcho);
    float measure();

  private:
    int pTrig;
    int pEcho;
    virtual ~RangeSensor();

};

#endif
