#ifndef Magnetometr_h
#define Magnetometr_h

#include <Arduino.h>
#include "systick.h"
#include "Sensor.h"

class Magnetometr : public Sensor {
  public:
    Magnetometr(uint8_t, uint32_t, uint8_t);
    virtual bool process();
  private:
    uint8_t   pin;
};


#endif
