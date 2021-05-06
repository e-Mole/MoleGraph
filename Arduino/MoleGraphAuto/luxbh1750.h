#ifndef LUXBH1750_h
#define LUXBH1750_h

#include <Arduino.h>
#include "timer.h"

class LUXBH1750 : public Sensor {
  public:
    LUXBH1750(uint32_t, uint8_t);
    virtual bool process();
    virtual float read(uint8_t);
  protected:
    uint8_t pin, pin_digi;
};

#endif
