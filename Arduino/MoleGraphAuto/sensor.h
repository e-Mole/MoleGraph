#ifndef Sensor_h
#define Sensor_h
#include <stdint.h>
#include <Arduino.h>
#include <Wire.h>

#define MAX_PORTS 4
#define NO_DATA 3.4028235E+38        //-1.0e-8f

extern volatile uint32_t newTime;
extern uint8_t PORTS[MAX_PORTS][4];

#define Action(x) ((uint32_t)(newTime - time) >= x)

class Sensor {
  bool dataReady = false;
  public:
    Sensor(uint32_t, uint8_t);
    virtual  ~Sensor();
    void start(uint32_t);
    void stop();
    bool processData();
    bool isDataReady() { return dataReady; } //will be set/reset in processData method
    virtual float read(uint8_t);
    virtual void  calibrate();

    uint32_t  period;
  protected:
    virtual bool  process();

    uint32_t  time;
    uint8_t   port;
    float     value = NO_DATA;
};

void      I2C_WriteByte(uint8_t addr, uint8_t data);
void      I2C_WriteRegister(uint8_t addr, uint8_t reg, uint8_t data);
uint8_t   I2C_ReadData8(uint8_t addr, uint8_t reg);
uint16_t  I2C_ReadData16BE(uint8_t addr, uint8_t reg);
uint16_t  I2C_ReadData16LE(uint8_t addr, uint8_t reg);

#endif
