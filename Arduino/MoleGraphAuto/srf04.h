#ifndef SRF04_h
#define SRF04_h

#include <Arduino.h>
#include "timer.h"

class SRF04 : public TimerAbstract {
  public:
    SRF04(uint32_t, uint8_t);
    virtual bool process();
    virtual float read(uint8_t);
    virtual void start(uint32_t);
  private:
    uint8_t   trigger;
    bool      active;
    uint32_t  delta;
    float     value_1, value_2;
    float     position, velocity, acceleration;
};

#endif
