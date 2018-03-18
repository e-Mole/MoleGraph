#ifndef Sensor_h
#define Sensor_h

#include <Arduino.h>
#include <Wire.h>
#include "systick.h"

#define NO_DATA         -1.0e-8f
#define MAX_PORTS       4

extern volatile uint32_t newTime;
extern uint8_t PORTS[MAX_PORTS][4];

#define Action(x) (newTime - time >= x) 

class Sensor {
  public:
    Sensor(uint32_t, uint8_t);
    virtual       ~Sensor();
    void          start(uint32_t);
    void          stop();
    virtual bool  process();
    virtual float read(uint8_t);
    virtual void  calibrate();

    uint8_t   channelCount = 0;
    uint32_t  period;
  protected:
    uint32_t  time;
    uint8_t   port;
    float     value = NO_DATA;
};

extern uint8_t sensorMask;
extern Sensor* sensor[MAX_PORTS];

void      I2C_WriteByte(uint8_t addr, uint8_t data);
void      I2C_WriteRegister(uint8_t addr, uint8_t reg, uint8_t data);
uint8_t   I2C_ReadData8(uint8_t addr, uint8_t reg);
uint16_t  I2C_ReadData16BE(uint8_t addr, uint8_t reg);
uint16_t  I2C_ReadData16LE(uint8_t addr, uint8_t reg);

#endif
