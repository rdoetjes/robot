#include "hmc5883l.h"

HMC5883L::HMC5883L(int devId) {
  this->devId = devId;
  this->fd = wiringPiI2CSetup(devId);
  checkRC(this->fd, "wiringPiI2CSetup");

  // Perform I2C work
  wiringPiI2CWriteReg8(this->fd, HMC5883L_REG_MODE, HMC5883L_MODE_CONTINUOUS);
}

void HMC5883L::checkRC(int fd, const char *text) {
  if (fd < 0) {
    printf("Error: %s - %d\n", text, fd);
    exit(-1);
  }
}

void HMC5883L::measure(hmc5883 *s){
  uint8_t msb = wiringPiI2CReadReg8(this->fd, HMC5883L_REG_MSB_X);
  uint8_t lsb = wiringPiI2CReadReg8(this->fd, HMC5883L_REG_LSB_X);
  short x = msb << 8 | lsb;

  msb = wiringPiI2CReadReg8(this->fd, HMC5883L_REG_MSB_Y);
  lsb = wiringPiI2CReadReg8(this->fd, HMC5883L_REG_LSB_Y);
  short y = msb << 8 | lsb;

  msb = wiringPiI2CReadReg8(this->fd, HMC5883L_REG_MSB_Z);
  lsb = wiringPiI2CReadReg8(this->fd, HMC5883L_REG_LSB_Z);
  short z = msb << 8 | lsb;

  double angle = atan2((double) y, (double)x) * (180 / PI) + 180;
  s->x = x;
  s->y = y;
  s->z = z;
  s->angle = angle;
}

HMC5883L::~HMC5883L(){
  //wiringPiI2CClose(this->fd);
}
