#ifndef VL53L0X_h
#define VL53L0X_h

#include <Arduino.h>
#include <Wire.h> 
#include "timer.h"

// =========================================================================
// SENSOR VERSION SELECTOR VL53L0X vs VL53L1X
// =========================================================================
// Uncomment the following line to use the new VL53L1X sensor.
// Comment it out to use the older VL53L0X sensor (original code).
// (#define USE_VL53L1X - moved to config.h)

#ifdef USE_VL53L1X
  #include "VL53L1X.h" 
#endif

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

#ifdef USE_VL53L1X
    VL53L1X sensorL1X;
#endif
};

#endif
