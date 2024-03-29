#ifndef AD_h
#define AD_h

#include <Arduino.h>
#include "timer.h"

class AD : public Sensor {
  public:
    AD(uint32_t, uint8_t);
    virtual bool process();
    virtual float read(uint8_t);
    virtual void calibrate();    
    float value2; //now public for use in conduct.cpp
  protected:
    uint8_t pin, pin_digi;
    uint32_t  counterTrigger = 0;
  private:
    //float value2;
    int32_t   offset = 0;      
};


#endif
