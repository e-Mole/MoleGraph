#ifndef SRF04_h
#define SRF04_h

#include <Arduino.h>
#include "systick.h"
#include "Timer.h"

class SRF04 : public TimerAbstract {
  public:
    SRF04(uint8_t, uint32_t, uint8_t);
    virtual bool process();
    virtual float read(uint8_t);
  private:
    uint8_t   trigger;
    bool      active;
    uint16_t  delta;
    float     value_1, value_2;
    float     velocity, acceleration;
};

#endif
