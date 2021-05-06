#ifndef DHT11_h
#define DHT11_h

#include <Arduino.h>
#include "timer.h"

class DHT11 : public Sensor {
  public:
    DHT11(uint32_t _period, uint8_t _port);
    virtual bool process();
    virtual float read(uint8_t);
  private:
    uint8_t   pin;
    uint8_t   active;     
    uint16_t  delta;
    uint8_t   data[5];
    bool      _lastresult; 
    uint32_t  _maxcycles; 
    bool      get();
    uint32_t  expectPulse(bool);
}; 

#endif
