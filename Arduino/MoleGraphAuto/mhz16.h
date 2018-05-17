#ifndef MHZ16_h
#define MHZ16_h

#include <Arduino.h>
#include "systick.h"
#include "Sensor.h"
#include "SoftSerial.h"
 
class MHZ16 : public Sensor {
  public:
    MHZ16(uint32_t, uint8_t);
    virtual bool process();
    virtual void calibrate();
    virtual float read(uint8_t);     
  private:
    SoftwareSerial serial;

    void     measure();
    uint8_t  parse(uint8_t *pbuf);

    static uint8_t  cmd_measure[9];
    static uint8_t  cmd_calibrateZero[9];
    float value2;
};

#endif
