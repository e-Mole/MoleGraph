#ifndef VEML6070_h
#define VEML6070_h

#include <Arduino.h>
#include "timer.h"

class VEML6070 : public Sensor {
  public:
    VEML6070(uint32_t, uint8_t);
    virtual bool process();
    virtual float read(uint8_t);
  protected:
    uint8_t pin, pin_digi;
  private:
    float value;
};

#endif