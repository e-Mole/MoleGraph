#ifndef LedBlink_h
#define LedBlink_h

#include <Arduino.h>
#include "timer.h"

class LedBlink : public Sensor {
  public:
    LedBlink(uint32_t, uint8_t);
    virtual bool process();
  private:
    uint8_t   pin;
};


#endif
