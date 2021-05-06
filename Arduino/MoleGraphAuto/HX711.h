#ifndef HX711_h
#define HX711_h

#include <Arduino.h>
#include "Sensor.h"

class HX711 : public Sensor {
  public:
    HX711(uint32_t, uint8_t);
    virtual bool  process();
    virtual float read(uint8_t);
    virtual void  calibrate();
  private:
    uint8_t   data, clk;
    uint8_t   gain = 3; // A x128
  protected:
    int32_t   getValue();
    int32_t   offset;
};


#endif
