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

float HMC5883L::heading(hmc5883 *s, float declinationAngle){
  this->measure(s);

  float heading = atan2((s->y - ((this->yMax + this->yMin) / 2.0)), (s->x - ((this->xMax + this->xMin) / 2.0)));
  heading += declinationAngle;

  // Correct for when signs are reversed.
  if (heading < 0) {
    heading += 2*PI;
  }

  // Check for wrap due to addition of declination.
  if (heading > 2*PI) {
    heading -= 2*PI;
  }

  return heading * 180/M_PI; // Convert radians to degrees.
}

void HMC5883L::calibrate(hmc5883 *s){
  std::cout << "slowy turn sensor 360 degrees several times\r\n";

  this->xMax = -10000;
  this->yMax = -10000;
  this->xMin = 10000;
  this->yMin = 10000;

  for (int i=0; i<100; i++) {
    this->measure(s);
    std::cout << s->x << " " << s->y << "\r\n";
    this->xMax = std::max(this->xMax, s->x);
    this->yMax = std::max(this->yMax, s->y);
    this->xMin = std::min(this->xMin, s->x);
    this->yMin = std::min(this->yMin, s->y);

    delay(100);
  }
  std::cout << this->xMax << " " << this->xMin << " " << this->yMax << " " << this->yMin << "\r\n";
}

HMC5883L::~HMC5883L(){
  //wiringPiI2CClose(this->fd);
}
