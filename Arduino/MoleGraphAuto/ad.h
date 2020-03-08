#ifndef AD_h
#define AD_h

#include <Arduino.h>
#include "systick.h"
#include "Sensor.h"

class AD : public Sensor {
  public:
    AD(uint32_t, uint8_t);
    virtual bool process();
    virtual float read(uint8_t);
  protected:
    uint8_t pin, pin_digi;
  private:
    float value2;
};


#endif
