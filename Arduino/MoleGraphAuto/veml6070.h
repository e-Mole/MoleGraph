#ifndef VEML6070_h
#define VEML6070_h

#include <Arduino.h>
#include <Wire.h>
#include "timer.h"

// I2C for VEML6070
// 0x38: Write command / Read LSB data
// 0x39: Read MSB data
#define VEML6070_ADDR_L  0x38 
#define VEML6070_ADDR_H  0x39 
// Settngs (IT=1T, SD=0, Reserved=1) -> 0x06
// Bit 0 (SD) = 0 (on)
// Bit 1 = 1 (allways)
// Bit 2-3 (IT) = 01 (1T integration - cca 125 ms -> 8 Hz)
#define VEML6070_CMD_1T  0x06

class VEML6070 : public Sensor {
  public:
    VEML6070(uint32_t, uint8_t);
    virtual bool process();
    virtual float read(uint8_t _spec);
    virtual void start(uint32_t _time);
  protected:
    uint8_t pin, pin_digi;
  private:
    float value;
    
    void initSensor();
    uint16_t readUV();
};

#endif
