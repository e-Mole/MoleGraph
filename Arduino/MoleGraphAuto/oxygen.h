#ifndef Oxygen_h
#define Oxygen_h

#include <Arduino.h>
#include "timer.h"

class Oxygen : public Sensor {
  public:
    Oxygen(uint32_t, uint8_t);
    virtual bool process();
    virtual float read(uint8_t);
  protected:
    uint8_t pin;
};

#endif
