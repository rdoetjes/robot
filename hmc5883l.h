/*
*/

#ifndef HMC5883L_H
#define HMNC5883L_H

#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <cstdio>

#define PI 3.14159265
#define HCM5883L_REG_CONFIG_A (0x00)
#define HCM5883L_REG_CONFIG_B (0x01)
#define HMC5883L_REG_MODE     (0x02)
#define HMC5883L_REG_MSB_X    (0x03)
#define HMC5883L_REG_LSB_X    (0x04)
#define HMC5883L_REG_MSB_Z    (0x05)
#define HMC5883L_REG_LSB_Z    (0x06)
#define HMC5883L_REG_MSB_Y    (0x07)
#define HMC5883L_REG_LSB_Y    (0x08)
#define HMC5883L_REG_STATUS   (0x09)
#define HMC5883L_REG_ID_A     (0x0a)
#define HMC5883L_REG_ID_B     (0x0b)
#define HMC5883L_REG_ID_C     (0x0c)
#define HMC5883L_MODE_CONTINUOUS (0x00)
#define HMC5883L_MODE_SINGLE     (0x01)

//convenience structure, used by drive function
struct hmc5883{
  float x, y, z, angle = 0.0;
};


class HMC5883L {

  public:
    HMC5883L(int devId);
    void measure(struct hmc5883 *s);
    virtual ~HMC5883L();
    void checkRC(int rc, const char *text);

  private:
    int devId;
    int fd;

};

#endif
