#ifndef PULSE_h
#define PULSE_h

#include <Arduino.h>
#include "timer.h"

class PULSE : public Sensor {
  public:
    PULSE(uint32_t, uint8_t);
    virtual bool process();
    virtual float read(uint8_t);
  protected:
    uint8_t   pin;
  private:
    unsigned int value2;
    unsigned int BPM;
    unsigned long bpmRead[5] = {0,0,0,0,0};
    bool counted = false;	
};


#endif
