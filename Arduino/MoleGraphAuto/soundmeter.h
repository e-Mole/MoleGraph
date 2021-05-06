#ifndef Sound_h
#define Sound_h

#include <Arduino.h>
#include "timer.h"

class Soundmeter : public Sensor {
  public:
    Soundmeter(uint32_t, uint8_t);
    virtual bool process();
    virtual float read(uint8_t);
  protected:
    uint8_t pin, pin_digi;
  private:
    float value2;
};

#endif
