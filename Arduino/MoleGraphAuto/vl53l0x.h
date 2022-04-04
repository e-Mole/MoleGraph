#ifndef VL53L0X_h
#define VL53L0X_h

#include <Arduino.h>
#include <Wire.h> 
#include "timer.h"

class VL53L0X : public Sensor {
  public:
    VL53L0X(uint32_t, uint8_t);
    virtual bool process();
    virtual float read(uint8_t);
    virtual void start(uint32_t);
  private:
    bool      active;
    uint32_t  delta;
    float     value_1, value_2;
    float     position, velocity, acceleration;
};


#endif
