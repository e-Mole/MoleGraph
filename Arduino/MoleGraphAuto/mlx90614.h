#ifndef MLX90614_h
#define MLX90614_h

#include <Arduino.h>
#include <Wire.h> 
#include "systick.h"
#include "Sensor.h"

class MLX90614 : public Sensor {
  public:
    MLX90614(uint32_t, uint8_t);
    virtual bool process();
    virtual float read(uint8_t);
  private:
    float value2;
};


#endif
