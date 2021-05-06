#ifndef Magnetometr_h
#define Magnetometr_h

#include <Arduino.h>
#include "timer.h"

class Magnetometr : public Sensor {
  public:
    Magnetometr(uint32_t, uint8_t);
    virtual bool process();
    virtual float read(uint8_t);
  protected:
    uint8_t pin, pin_digi;
  private:
    float value2;
};

#endif
