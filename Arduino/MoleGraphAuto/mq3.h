#ifndef MQ3_h
#define MQ3_h

#include <Arduino.h>
#include "timer.h"

class MQ3 : public Sensor {
  public:
    MQ3(uint32_t, uint8_t);
    virtual bool process();
    virtual float read(uint8_t);
    virtual void calibrate();
  protected:
    uint8_t pin, pin_digi;
  private:
    float value2;
    int32_t   offset = 0;	
};


#endif
