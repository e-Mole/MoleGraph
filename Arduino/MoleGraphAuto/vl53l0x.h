#ifndef VL53L0X_h
#define VL53L0X_h

#include <Arduino.h>
#include <Wire.h> 
#include "systick.h"
#include "Sensor.h"

class VL53L0X : public Sensor {
  public:
    VL53L0X(uint8_t, uint32_t, uint8_t);
    virtual bool process();
  private:
    bool      active;
    uint16_t  delta;
};


#endif
