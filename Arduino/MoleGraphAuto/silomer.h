#ifndef Silomer_h
#define Silomer_h

#include <Arduino.h>
#include "systick.h"
#include "Sensor.h"

class Silomer : public Sensor {
  public:
    Silomer(uint32_t, uint8_t);
    virtual bool process();
    virtual void calibrate();
  private:
    int32_t   getValue();
    uint8_t   data, clk;
    uint8_t   gain = 1; // A x128
    int32_t   offset;
    float     scale;
};


#endif
