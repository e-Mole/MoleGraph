#ifndef VOLT25_h
#define VOLT25_h

#include <Arduino.h>
#include "timer.h"

class VOLT25 : public Sensor {
  public:
    VOLT25(uint32_t, uint8_t);
    virtual bool process();
    virtual float read(uint8_t);
  protected:
    uint8_t   pin;
};


#endif
