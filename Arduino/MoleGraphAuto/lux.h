#ifndef LUX_h
#define LUX_h

#include <Arduino.h>
#include "systick.h"
#include "AD.h"

class Lux : public AD {
  public:
    Lux(uint32_t, uint8_t);
    virtual float read();
};

#endif
