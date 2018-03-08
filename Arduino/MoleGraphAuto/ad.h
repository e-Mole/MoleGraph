#ifndef AD_h
#define AD_h

#include <Arduino.h>
#include "systick.h"
#include "Sensor.h"

class AD : public Sensor {
  public:
    AD(uint8_t, uint32_t, uint8_t);
    virtual bool process();
    virtual float read(uint8_t);
  private:
    uint8_t   pin;
};


#endif
