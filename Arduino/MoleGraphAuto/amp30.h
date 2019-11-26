#ifndef AMP30_h
#define AMP30_h

#include <Arduino.h>
#include "systick.h"
#include "Sensor.h"

class AMP30 : public Sensor {
  public:
    AMP30(uint32_t, uint8_t);
    virtual bool process();
    virtual float read(uint8_t);
  protected:
    uint8_t   pin;
};


#endif
