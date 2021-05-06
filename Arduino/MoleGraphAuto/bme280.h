#ifndef BME280_h
#define BME280_h

#include <Arduino.h>
#include "timer.h"

class BME280 : public Sensor {
  public:
    BME280(uint32_t, uint8_t);
    virtual bool process();
    virtual float read(uint8_t);
  protected:
    uint8_t pin, pin_digi;
  private:
    float value2, value3;
    virtual float calcAltitude();
    virtual float calcDewPoint();
};

#endif
