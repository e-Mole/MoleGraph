#ifndef AMP5_h
#define AMP5_h

#include <Arduino.h>
#include "timer.h"

class AMP5 : public Sensor {
  public:
    AMP5(uint32_t, uint8_t);
    virtual bool process();
    virtual float read(uint8_t);
  protected:
    uint8_t   pin;
};


#endif
