#ifndef DS18B20_h
#define DS18B20_h

#include <Arduino.h>
#include <OneWire.h> 
#include "systick.h"
#include "Sensor.h"

class DS18B20 : public Sensor {
  public:
    DS18B20(uint8_t _type, uint32_t _period, uint8_t _port, uint8_t _resolution = 3);
    virtual bool process();
  private:
    uint8_t   pin;
    bool      active;     
    uint8_t   resolution;
    uint16_t  delta;
    OneWire   *ds;
    void      startTemp();
    bool      get();
    void      setResolution();
}; 

#endif
