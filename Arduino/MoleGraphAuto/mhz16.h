#ifndef MHZ16_h
#define MHZ16_h

#include <Arduino.h>
#include "timer.h"

class MHZ16 : public Timer {
  public:
    MHZ16(uint32_t, uint8_t);
    virtual float read(uint8_t);
};


#endif
