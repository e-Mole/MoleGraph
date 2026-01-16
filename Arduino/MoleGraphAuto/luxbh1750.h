#ifndef LUXBH1750_h
#define LUXBH1750_h

#include <Arduino.h>
#include <Wire.h>
#include "timer.h"

// --- Sensor settings ---
#define BH1750_I2CADDR 0x23
// Příkazy pro nastavení režimu
#define BH1750_POWER_ON 0x01
#define BH1750_RESET    0x07
#define BH1750_CONTINUOUS_HIGH_RES_MODE  0x10 // max. sample rate 8 Hz, resolution 1 lux
//#define BH1750_CONTINUOUS_LOW_RES_MODE  0x13 // max. sample rate 60 Hz, resolution 4 lux

class LUXBH1750 : public Sensor {
  public:
    LUXBH1750(uint32_t, uint8_t);
    virtual bool process();
    virtual float read(uint8_t);
  protected:
    uint8_t pin, pin_digi;
  private:
    void configure(uint8_t mode);
    uint16_t readLightLevel();
};

#endif
