#ifndef LUX_h
#define LUX_h

#include <Arduino.h>
#include "systick.h"
#include "Sensor.h"

class Lux : public Sensor {
  public:
    Lux(uint8_t, uint32_t, uint8_t);
    virtual bool process();
  private:
    uint8_t   pin;
    float     scale;
};

#endif
