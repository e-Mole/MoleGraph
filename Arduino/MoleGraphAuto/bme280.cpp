#include "bme280.h"
#include <math.h> // needed for pow() and log()

#define SEALEVELPRESSURE_HPA (1013.25)

BME280::BME280(uint32_t _period, uint8_t _port) : Sensor(_period, _port) {
	//BME280 settings
    _i2caddr = BME280_ADDRESS;
    temp_c = 0.0;
    press_hpa = 0.0;
    hum_pct = 0.0;
    
    // BME280 initializing
    initBME();  
}

bool BME280::process() {
  if (Action(period)) {
    // Reading new data and fill variables (compensation inside)
    readData();
    
    time += period;
    return 1;
  }
  return 0;
}

float BME280::read(uint8_t _spec) {
  float result = NO_DATA;
  switch (_spec) {
    case 0: result = press_hpa; break;      // pressure hPa
    case 1: result = temp_c; break;         // temperature °C
    case 2: result = hum_pct; break;        // humidity %
    case 3: result = calcAltitude(); break; // altitude m
    case 4: result = calcDewPoint(); break; // dewpoint °C
  }
  return result;
}

float BME280::calcAltitude() {
    // use press_hpa
    return 44330.0 * (1.0 - pow(press_hpa / SEALEVELPRESSURE_HPA, 0.1903));
}

float BME280::calcDewPoint() {
    //use temp_c a hum_pct
    float a = 17.625;
    float b = 243.04;
    // treatement log(0)
    if (hum_pct < 0.1) return 0.0; 
    
    float alpha = log(hum_pct/100.0) + ((a * temp_c) / (b + temp_c));
    return (b * alpha) / (a - alpha);
}

bool BME280::initBME() {
    Wire.begin();
    
    if (read8(BME280_REGISTER_CHIPID) != 0x60) {
        return false;
    }
    
    readSensorCoefficients();
    
    // Set default oversampling
    write8(BME280_REGISTER_CONTROLHUMID, 0x01); 
    write8(BME280_REGISTER_CONTROL, 0x3F); // Normal mode, temp/press oversampling
    
    return true;
}

void BME280::readData() {
    // 1. TEMPERATURE
    int32_t adc_T = read24(BME280_REGISTER_TEMPDATA);
    adc_T >>= 4;
    int32_t var1_t, var2_t;
    var1_t = ((((adc_T >> 3) - ((int32_t)cal_data.dig_T1 << 1))) * ((int32_t)cal_data.dig_T2)) >> 11;
    var2_t = (((((adc_T >> 4) - ((int32_t)cal_data.dig_T1)) * ((adc_T >> 4) - ((int32_t)cal_data.dig_T1))) >> 12) * ((int32_t)cal_data.dig_T3)) >> 14;
    t_fine = var1_t + var2_t;
    float T = (t_fine * 5 + 128) >> 8;
    temp_c = T / 100.0;
    // Possible temp correction (in all my BME280 sensors -2 °C);
    temp_c -= 2.0; 

    // 2. PRESSURE
    int32_t adc_P = read24(BME280_REGISTER_PRESSUREDATA);
    adc_P >>= 4;
    int64_t var1_p, var2_p, p;
    var1_p = ((int64_t)t_fine) - 128000;
    var2_p = var1_p * var1_p * (int64_t)cal_data.dig_P6;
    var2_p = var2_p + ((var1_p * (int64_t)cal_data.dig_P5) << 17);
    var2_p = var2_p + (((int64_t)cal_data.dig_P4) << 35);
    var1_p = ((var1_p * var1_p * (int64_t)cal_data.dig_P3) >> 8) + ((var1_p * (int64_t)cal_data.dig_P2) << 12);
    var1_p = (((((int64_t)1) << 47) + var1_p)) * ((int64_t)cal_data.dig_P1) >> 33;

    if (var1_p == 0) {
        press_hpa = 0.0;
    } else {
        p = 1048576 - adc_P;
        p = (((p << 31) - var2_p) * 3125) / var1_p;
        var1_p = (((int64_t)cal_data.dig_P9) * (p >> 13) * (p >> 13)) >> 25;
        var2_p = (((int64_t)cal_data.dig_P8) * p) >> 19;
        p = ((p + var1_p + var2_p) >> 8) + (((int64_t)cal_data.dig_P7) << 4);
        press_hpa = (float)p / 256.0 / 100.0; // Pa -> hPa
    }

    // 3. HUMIDITY
    int32_t adc_H = read16(BME280_REGISTER_HUMIDDATA);
    int32_t v_x1_u32r;
    v_x1_u32r = (t_fine - ((int32_t)76800));
    v_x1_u32r = (((((adc_H << 14) - (((int32_t)cal_data.dig_H4) << 20) - (((int32_t)cal_data.dig_H5) * v_x1_u32r)) +
                   ((int32_t)16384)) >> 15) * (((((((v_x1_u32r * ((int32_t)cal_data.dig_H6)) >> 10) *
                   (((v_x1_u32r * ((int32_t)cal_data.dig_H3)) >> 11) + ((int32_t)32768))) >> 10) + ((int32_t)2097152)) *
                   ((int32_t)cal_data.dig_H2) + 8192) >> 14));
    v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) * ((int32_t)cal_data.dig_H1)) >> 4));
    v_x1_u32r = (v_x1_u32r < 0 ? 0 : v_x1_u32r);
    v_x1_u32r = (v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r);
    hum_pct = (float)(v_x1_u32r >> 12) / 1024.0;
}

void BME280::readSensorCoefficients() {
    cal_data.dig_T1 = read16_LE(BME280_DIG_T1_REG);
    cal_data.dig_T2 = readS16_LE(BME280_DIG_T2_REG);
    cal_data.dig_T3 = readS16_LE(BME280_DIG_T3_REG);

    cal_data.dig_P1 = read16_LE(BME280_DIG_P1_REG);
    cal_data.dig_P2 = readS16_LE(BME280_DIG_P2_REG);
    cal_data.dig_P3 = readS16_LE(BME280_DIG_P3_REG);
    cal_data.dig_P4 = readS16_LE(BME280_DIG_P4_REG);
    cal_data.dig_P5 = readS16_LE(BME280_DIG_P5_REG);
    cal_data.dig_P6 = readS16_LE(BME280_DIG_P6_REG);
    cal_data.dig_P7 = readS16_LE(BME280_DIG_P7_REG);
    cal_data.dig_P8 = readS16_LE(BME280_DIG_P8_REG);
    cal_data.dig_P9 = readS16_LE(BME280_DIG_P9_REG);

    cal_data.dig_H1 = read8(BME280_DIG_H1_REG);
    cal_data.dig_H2 = readS16_LE(BME280_DIG_H2_REG);
    cal_data.dig_H3 = read8(BME280_DIG_H3_REG);
    cal_data.dig_H4 = (read8(BME280_DIG_H4_REG) << 4) | (read8(BME280_DIG_H4_REG+1) & 0xF);
    cal_data.dig_H5 = (read8(BME280_DIG_H5_REG+1) << 4) | (read8(BME280_DIG_H5_REG) >> 4);
    cal_data.dig_H6 = (int8_t)read8(BME280_DIG_H6_REG);
}

// --- I2C Helpers ---

void BME280::write8(byte reg, byte value) {
    Wire.beginTransmission((uint8_t)_i2caddr);
    Wire.write((uint8_t)reg);
    Wire.write((uint8_t)value);
    Wire.endTransmission();
}

uint8_t BME280::read8(byte reg) {
    Wire.beginTransmission((uint8_t)_i2caddr);
    Wire.write((uint8_t)reg);
    Wire.endTransmission();
    Wire.requestFrom((uint8_t)_i2caddr, (byte)1);
    return Wire.read();
}

uint16_t BME280::read16(byte reg) {
    Wire.beginTransmission((uint8_t)_i2caddr);
    Wire.write((uint8_t)reg);
    Wire.endTransmission();
    Wire.requestFrom((uint8_t)_i2caddr, (byte)2);
    return (Wire.read() << 8) | Wire.read();
}

uint16_t BME280::read16_LE(byte reg) {
    uint16_t temp = read16(reg);
    return (temp >> 8) | (temp << 8);
}

int16_t BME280::readS16(byte reg) { return (int16_t)read16(reg); }
int16_t BME280::readS16_LE(byte reg) { return (int16_t)read16_LE(reg); }

uint32_t BME280::read24(byte reg) {
    Wire.beginTransmission((uint8_t)_i2caddr);
    Wire.write((uint8_t)reg);
    Wire.endTransmission();
    Wire.requestFrom((uint8_t)_i2caddr, (byte)3);
    uint32_t value = Wire.read();
    value <<= 8;
    value |= Wire.read();
    value <<= 8;
    value |= Wire.read();
    return value;
}
