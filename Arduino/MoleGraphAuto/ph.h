#ifndef PH_h
#define PH_h

#include <Arduino.h>
#include "systick.h"
#include "Sensor.h"

class PH : public Sensor {
  public:
    PH(uint32_t, uint8_t);
    virtual bool process();
    virtual float read(uint8_t);
  protected:
    uint8_t pin, pin_digi;
  private:
    float value2;	
};


#endif
