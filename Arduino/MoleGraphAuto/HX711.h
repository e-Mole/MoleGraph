#ifndef HX711_h
#define HX711_h

#include <Arduino.h>
#include "systick.h"
#include "Sensor.h"

class HX711 : public Sensor {
  public:
    HX711(uint8_t, uint32_t, uint8_t);
    virtual bool process();
    virtual void calibrate();
  private:
    int32_t   getValue();
    uint8_t   data, clk;
    uint8_t   gain = 3; // A x128
    int32_t   offset;
};


#endif
