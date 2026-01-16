#ifndef BME280_h
#define BME280_h

#include <Arduino.h>
#include "timer.h"

// --- BME280_I2C registers) ---
#define BME280_ADDRESS      0x76  // default address (or 0x77)

#define BME280_DIG_T1_REG   0x88
#define BME280_DIG_T2_REG   0x8A
#define BME280_DIG_T3_REG   0x8C
#define BME280_DIG_P1_REG   0x8E
#define BME280_DIG_P2_REG   0x90
#define BME280_DIG_P3_REG   0x92
#define BME280_DIG_P4_REG   0x94
#define BME280_DIG_P5_REG   0x96
#define BME280_DIG_P6_REG   0x98
#define BME280_DIG_P7_REG   0x9A
#define BME280_DIG_P8_REG   0x9C
#define BME280_DIG_P9_REG   0x9E
#define BME280_DIG_H1_REG   0xA1
#define BME280_DIG_H2_REG   0xE1
#define BME280_DIG_H3_REG   0xE3
#define BME280_DIG_H4_REG   0xE4
#define BME280_DIG_H5_REG   0xE5
#define BME280_DIG_H6_REG   0xE7

#define BME280_REGISTER_CHIPID       0xD0
#define BME280_REGISTER_VERSION      0xD1
#define BME280_REGISTER_SOFTRESET    0xE0
#define BME280_REGISTER_CAL26        0xE1
#define BME280_REGISTER_CONTROLHUMID 0xF2
#define BME280_REGISTER_CONTROL      0xF4
#define BME280_REGISTER_CONFIG       0xF5
#define BME280_REGISTER_PRESSUREDATA 0xF7
#define BME280_REGISTER_TEMPDATA     0xFA
#define BME280_REGISTER_HUMIDDATA    0xFD

// Calibration data structure
struct BME280_Calibration_Data {
    uint16_t dig_T1;
    int16_t  dig_T2, dig_T3;
    uint16_t dig_P1;
    int16_t  dig_P2, dig_P3, dig_P4, dig_P5, dig_P6, dig_P7, dig_P8, dig_P9;
    uint8_t  dig_H1, dig_H3;
    int16_t  dig_H2, dig_H4, dig_H5;
    int8_t   dig_H6;
};

class BME280 : public Sensor {
  public:
    BME280(uint32_t, uint8_t);
    virtual bool process();
    virtual float read(uint8_t);
  protected:
    uint8_t pin, pin_digi;
  private:
    float temp_c;    // Teplota ve °C
    float press_hpa; // Tlak v hPa
    float hum_pct;   // Vlhkost v %
    
    // Calibration data
    BME280_Calibration_Data cal_data;
    int32_t t_fine;
    uint8_t _i2caddr;

    // Calculations
    float calcAltitude();
    float calcDewPoint();

    // Inthernal methods for BME280
    bool initBME();
    void readSensorCoefficients();
    void readData(); // Reading all BME280 sensor data
    
    // I2C functions
    void write8(byte reg, byte value);
    uint8_t read8(byte reg);
    uint16_t read16(byte reg);
    uint16_t read16_LE(byte reg);
    int16_t readS16(byte reg);
    int16_t readS16_LE(byte reg);
    uint32_t read24(byte reg);
};

#endif
