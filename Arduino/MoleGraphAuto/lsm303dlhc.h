#ifndef LSM303DLHC_h
#define LSM303DLHC_h

#include <Arduino.h>
#include "timer.h"

class LSM303DLHC : public Sensor {
  public:
    LSM303DLHC(uint32_t, uint8_t);
    virtual bool process();
    virtual float read(uint8_t);
  protected:
    uint8_t pin, pin_digi;
  private:
    float value2, aX, aY, aZ, mX, mY, mZ, heading;
	  double fXg, fYg, fZg, pitch, roll;
};

#endif
