#ifndef LUX_h
#define LUX_h

#include <Arduino.h>
#include "timer.h"

class Lux : public Sensor {
  public:
    Lux(uint32_t, uint8_t);
    virtual bool process();
    virtual float read(uint8_t);
  protected:
    uint8_t pin, pin_digi;
  private:
    float     scale;
    float     value2;
};

#endif
