#ifndef Force_h
#define Force_h

#include <Arduino.h>
#include "systick.h"
#include "hx711.h"

class Force : public HX711 {
  public:
    Force(uint32_t, uint8_t);
    virtual float read(uint8_t);
  private:
    float     scale;
};


#endif
