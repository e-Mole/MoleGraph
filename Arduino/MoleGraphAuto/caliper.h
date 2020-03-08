#ifndef CALIPER_h
#define CALIPER_h

#include <Arduino.h>
#include "systick.h"
#include "Sensor.h"

class CALIPER : public Sensor {
  public:
    CALIPER(uint32_t, uint8_t);
    virtual bool process();
    virtual float read(uint8_t);
  protected:
    uint8_t pin_clk, pin_data;
  private:
    //float value2;
	unsigned long time_now;
    virtual void decodeBits();
};


#endif
