#include "LSM303DLHC.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

LSM303DLHC::LSM303DLHC(uint32_t _period, uint8_t _port) : Sensor(_period, _port) {
  acc_raw_x = acc_raw_y = acc_raw_z = 0;
  mag_raw_x = mag_raw_y = mag_raw_z = 0;
  
  fXg = 0; fYg = 0; fZg = 0;
  alpha = 0.5; // Low Pass Filter coefficient

  _autoRangeEnabled = true;
  _magGain = LSM303_MAGGAIN_1_3; 
  _accRange = LSM303_ACCEL_RANGE_2G; 
}

void LSM303DLHC::enableAutoRange(bool enabled) {
  _autoRangeEnabled = enabled;
}

void LSM303DLHC::start(uint32_t now) {
  Sensor::start(now);
  Wire.begin();
  initAccel();
  initMag();
}

// --- INITIALIZATION ---

void LSM303DLHC::initAccel() {
  Wire.beginTransmission(LSM303_ADDR_ACCEL);
  Wire.write(LSM303_REG_ACCEL_CTRL1);
  // Speed... (for 12bit values)
  // Wire.write(0x27); // 10 Hz
  // Wire.write(0x47); // 50 Hz
  // Wire.write(0x57); // 100 Hz (default)
  Wire.write(0x67);    // 200 Hz (for impact measurement)
  // Wire.write(0x77); // 400 Hz (highspeed, need I2C on 400kHz)
  Wire.endTransmission();

  setAccelRange(_accRange); 
}

void LSM303DLHC::initMag() {
  setMagGain(_magGain); 

  Wire.beginTransmission(LSM303_ADDR_MAG);
  Wire.write(LSM303_REG_MAG_MR);
  Wire.write(0x00); // Continuous Conversion
  Wire.endTransmission();
}

void LSM303DLHC::setAccelRange(uint8_t range) {
  _accRange = range;
  Wire.beginTransmission(LSM303_ADDR_ACCEL);
  Wire.write(LSM303_REG_ACCEL_CTRL4);
  Wire.write(_accRange); 
  Wire.endTransmission();

  // Set conversion factor based on range
  switch(range) {
    case LSM303_ACCEL_RANGE_2G:  _lsm303Accel_MG_LSB = 0.001F; break; 
    case LSM303_ACCEL_RANGE_4G:  _lsm303Accel_MG_LSB = 0.002F; break; 
    case LSM303_ACCEL_RANGE_8G:  _lsm303Accel_MG_LSB = 0.004F; break; 
    case LSM303_ACCEL_RANGE_16G: _lsm303Accel_MG_LSB = 0.012F; break; 
  }
}

void LSM303DLHC::setMagGain(uint8_t gain) {
  _magGain = gain;
  Wire.beginTransmission(LSM303_ADDR_MAG);
  Wire.write(LSM303_REG_MAG_CRB);
  Wire.write(_magGain);
  Wire.endTransmission();

  // Set conversion factors based on gain (XY and Z differ)
  switch(gain) {
    case LSM303_MAGGAIN_1_3: _lsm303Mag_Gauss_LSB_XY = 1100.0F; _lsm303Mag_Gauss_LSB_Z = 980.0F; break;
    case LSM303_MAGGAIN_1_9: _lsm303Mag_Gauss_LSB_XY = 855.0F;  _lsm303Mag_Gauss_LSB_Z = 760.0F; break;
    case LSM303_MAGGAIN_2_5: _lsm303Mag_Gauss_LSB_XY = 670.0F;  _lsm303Mag_Gauss_LSB_Z = 600.0F; break;
    case LSM303_MAGGAIN_4_0: _lsm303Mag_Gauss_LSB_XY = 450.0F;  _lsm303Mag_Gauss_LSB_Z = 400.0F; break;
    case LSM303_MAGGAIN_4_7: _lsm303Mag_Gauss_LSB_XY = 400.0F;  _lsm303Mag_Gauss_LSB_Z = 355.0F; break;
    case LSM303_MAGGAIN_5_6: _lsm303Mag_Gauss_LSB_XY = 330.0F;  _lsm303Mag_Gauss_LSB_Z = 295.0F; break;
    case LSM303_MAGGAIN_8_1: _lsm303Mag_Gauss_LSB_XY = 230.0F;  _lsm303Mag_Gauss_LSB_Z = 205.0F; break;
  }
}

bool LSM303DLHC::process() {
  if (Action(period)) {
      bool accel_ok = readAccel();
      bool mag_ok   = readMag();
    
      // 1. Convert Accel to SI units (m/s^2)
      float accel_factor = _lsm303Accel_MG_LSB * SENSORS_GRAVITY_STANDARD;
      if (accel_ok) {
        aX = (float)acc_raw_x * accel_factor;
        aY = (float)acc_raw_y * accel_factor;
        aZ = (float)acc_raw_z * accel_factor;
      }
      // 2. Convert Mag to SI units (uT)
      if (mag_ok) {
        mX = ((float)mag_raw_x / _lsm303Mag_Gauss_LSB_XY) * SENSORS_GAUSS_TO_MICROTESLA;
        mY = ((float)mag_raw_y / _lsm303Mag_Gauss_LSB_XY) * SENSORS_GAUSS_TO_MICROTESLA;
        mZ = ((float)mag_raw_z / _lsm303Mag_Gauss_LSB_Z)  * SENSORS_GAUSS_TO_MICROTESLA;
      }
      // 3. Totals
      value  = sqrt(aX*aX + aY*aY + aZ*aZ); // Accel Total
      value2 = sqrt(mX*mX + mY*mY + mZ*mZ); // Mag Total
    
      // 4. Low Pass Filter
      fXg = aX * alpha + (fXg * (1.0 - alpha));
      fYg = aY * alpha + (fYg * (1.0 - alpha));
      fZg = aZ * alpha + (fZg * (1.0 - alpha));
    
      // 5. Heading
      heading = (atan2(mY, mX) * 180.0) / M_PI;
      if (heading < 0) {
        heading = 360 + heading;
      }
    
      // 6. Roll & Pitch
      roll  = (atan2(-fYg, fZg) * 180.0) / M_PI;
      pitch = (atan2(fXg, sqrt(fYg*fYg + fZg*fZg)) * 180.0) / M_PI;
    
      time += period;
    return 1;
  }
  return 0;
}

bool LSM303DLHC::readAccel() {
  Wire.beginTransmission(LSM303_ADDR_ACCEL);
  Wire.write(LSM303_REG_ACCEL_OUT_X_L | 0x80);
  Wire.endTransmission();
  
  Wire.requestFrom(LSM303_ADDR_ACCEL, 6);
  if (Wire.available() == 6) {
    uint8_t xlo = Wire.read(); uint8_t xhi = Wire.read();
    uint8_t ylo = Wire.read(); uint8_t yhi = Wire.read();
    uint8_t zlo = Wire.read(); uint8_t zhi = Wire.read();

    acc_raw_x = (int16_t)((uint16_t)xlo | ((uint16_t)xhi << 8)) >> 4;
    acc_raw_y = (int16_t)((uint16_t)ylo | ((uint16_t)yhi << 8)) >> 4;
    acc_raw_z = (int16_t)((uint16_t)zlo | ((uint16_t)zhi << 8)) >> 4;

    if (_autoRangeEnabled) {
      // Saturation check (Manual check instead of abs() for safety/performance)
      bool saturated = false;
      if (acc_raw_x > 2040 || acc_raw_x < -2040) saturated = true;
      if (acc_raw_y > 2040 || acc_raw_y < -2040) saturated = true;
      if (acc_raw_z > 2040 || acc_raw_z < -2040) saturated = true;

      if (saturated) {
        uint8_t nextRange = _accRange;
        switch(_accRange) {
          case LSM303_ACCEL_RANGE_2G:  nextRange = LSM303_ACCEL_RANGE_4G;  break;
          case LSM303_ACCEL_RANGE_4G:  nextRange = LSM303_ACCEL_RANGE_8G;  break;
          case LSM303_ACCEL_RANGE_8G:  nextRange = LSM303_ACCEL_RANGE_16G; break;
        }
        if (nextRange != _accRange){
          setAccelRange(nextRange);
          return false; //saturated data (dont use data - typicaly peak)
        }
      }
    }
    return true; // data o.k.
  }
  return false; // I2C bus eror - no data
}

bool LSM303DLHC::readMag() {
  Wire.beginTransmission(LSM303_ADDR_MAG);
  Wire.write(LSM303_REG_MAG_OUT_X_H);
  Wire.endTransmission();

  Wire.requestFrom(LSM303_ADDR_MAG, 6);
  if (Wire.available() == 6) {
    // Note: X, Z, Y order and Big Endian
    uint8_t xhi = Wire.read(); uint8_t xlo = Wire.read();
    uint8_t zhi = Wire.read(); uint8_t zlo = Wire.read();
    uint8_t yhi = Wire.read(); uint8_t ylo = Wire.read();

    mag_raw_x = (int16_t)((uint16_t)xlo | ((uint16_t)xhi << 8));
    mag_raw_y = (int16_t)((uint16_t)ylo | ((uint16_t)yhi << 8));
    mag_raw_z = (int16_t)((uint16_t)zlo | ((uint16_t)zhi << 8));
    
    if (_autoRangeEnabled) {
      // Saturation check (Manual check)
      bool saturated = false;
      if (mag_raw_x > 2040 || mag_raw_x < -2040) saturated = true;
      if (mag_raw_y > 2040 || mag_raw_y < -2040) saturated = true;
      if (mag_raw_z > 2040 || mag_raw_z < -2040) saturated = true;

      if (saturated) {
        uint8_t nextGain = _magGain;
        switch(_magGain) {
          case LSM303_MAGGAIN_1_3: nextGain = LSM303_MAGGAIN_1_9; break;
          case LSM303_MAGGAIN_1_9: nextGain = LSM303_MAGGAIN_2_5; break;
          case LSM303_MAGGAIN_2_5: nextGain = LSM303_MAGGAIN_4_0; break;
          case LSM303_MAGGAIN_4_0: nextGain = LSM303_MAGGAIN_4_7; break;
          case LSM303_MAGGAIN_4_7: nextGain = LSM303_MAGGAIN_5_6; break;
          case LSM303_MAGGAIN_5_6: nextGain = LSM303_MAGGAIN_8_1; break;
        }
        if (nextGain != _magGain) {
          setMagGain(nextGain);
          return false; //saturated data (dont use data - typicaly peak)
        }
      }
    }
    return true; // data o.k.
  }
  return false; // I2C bus eror - no data
}

float LSM303DLHC::read(uint8_t _spec) {
  float result = NO_DATA;
  switch (_spec) {
    case 0: result = value; break;   // accel total in m/s2
    case 1: result = value2; break;  // mag total in uT
    case 2: result = aX; break;      // accel X in m/s2
    case 3: result = aY; break;      // accel Y in m/s2
    case 4: result = aZ; break;      // accel Z in m/s2
    case 5: result = mX; break;      // mag X in uT
    case 6: result = mY; break;      // mag Y in uT
    case 7: result = mZ; break;      // mag Z in uT
    case 8: result = heading; break; // heading in deg
    case 9: result = roll; break;    // roll in deg
    case 10: result = pitch; break;  // pitch in deg
  }
  return result;
}
