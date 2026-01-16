#ifndef LSM303DLHC_h
#define LSM303DLHC_h

#include <Arduino.h>
#include <Wire.h>
#include <math.h> // Required for sqrt, atan2 (standard math library)
#include "timer.h"

// --- I2C ADDRESSES ---
// The LSM303DLHC is a combo chip: Accel and Mag have separate addresses.
#define LSM303_ADDR_ACCEL    (0x32 >> 1) // 0x19 (Linear Acceleration Sensor)
#define LSM303_ADDR_MAG      (0x3C >> 1) // 0x1E (Magnetic Field Sensor)

// --- REGISTERS ---
#define LSM303_REG_ACCEL_CTRL1    0x20 // Control register 1 (Data rate, Enable axes)
#define LSM303_REG_ACCEL_CTRL4    0x23 // Control register 4 (Block update, Little/Big endian, Full scale, High res)
#define LSM303_REG_ACCEL_OUT_X_L  0x28 // Output Register X Low (Start of data)
#define LSM303_REG_MAG_CRB        0x01 // Configuration Register B (Gain setting)
#define LSM303_REG_MAG_MR         0x02 // Mode Register (Continuous/Single/Sleep)
#define LSM303_REG_MAG_OUT_X_H    0x03 // Output Register X High (Start of data)

// --- MAGNETOMETER GAIN SETTINGS ---
// Sets the range of the magnetic sensor. Lower Gauss = Higher Sensitivity.
// GN bits (7-5) in CRB_REG_M.
#define LSM303_MAGGAIN_1_3        0x20  // +/- 1.3 Gauss (Default) | LSB: XY=1100, Z=980
#define LSM303_MAGGAIN_1_9        0x40  // +/- 1.9 Gauss           | LSB: XY=855,  Z=760
#define LSM303_MAGGAIN_2_5        0x60  // +/- 2.5 Gauss           | LSB: XY=670,  Z=600
#define LSM303_MAGGAIN_4_0        0x80  // +/- 4.0 Gauss           | LSB: XY=450,  Z=400
#define LSM303_MAGGAIN_4_7        0xA0  // +/- 4.7 Gauss           | LSB: XY=400,  Z=355
#define LSM303_MAGGAIN_5_6        0xC0  // +/- 5.6 Gauss           | LSB: XY=330,  Z=295
#define LSM303_MAGGAIN_8_1        0xE0  // +/- 8.1 Gauss           | LSB: XY=230,  Z=205

// --- ACCELEROMETER RANGE SETTINGS ---
// Sets the full scale range (FS). Higher G = Lower Sensitivity.
// Includes High Resolution bit (HR) set to 1 (bit 3).
#define LSM303_ACCEL_RANGE_2G     0x08 // +/- 2G  (Default) | Sensitivity: ~1 mg/LSB
#define LSM303_ACCEL_RANGE_4G     0x18 // +/- 4G            | Sensitivity: ~2 mg/LSB
#define LSM303_ACCEL_RANGE_8G     0x28 // +/- 8G            | Sensitivity: ~4 mg/LSB
#define LSM303_ACCEL_RANGE_16G    0x38 // +/- 16G           | Sensitivity: ~12 mg/LSB

// --- PHYSICAL CONSTANTS ---
#define SENSORS_GRAVITY_STANDARD    9.80665F // Earth's gravity in m/s^2
#define SENSORS_GAUSS_TO_MICROTESLA 100.0F   // 1 Gauss = 100 uT

class LSM303DLHC : public Sensor {
  public:
    LSM303DLHC(uint32_t, uint8_t);
    virtual bool process();
    virtual float read(uint8_t);
    virtual void start(uint32_t _time);
    
    // Enable automatic range switching to prevent saturation
    void enableAutoRange(bool enabled);    
  protected:
    uint8_t pin, pin_digi;
  private:
    void initAccel();
    void initMag();
    bool readAccel();
    bool readMag();
    
    void setMagGain(uint8_t gain);
    void setAccelRange(uint8_t range);

    // Raw data (direct from registers)
    int16_t acc_raw_x, acc_raw_y, acc_raw_z;
    int16_t mag_raw_x, mag_raw_y, mag_raw_z;
    
    // Converted physical values (SI units: m/s^2, uT)
    float aX, aY, aZ; 
    float mX, mY, mZ; 
    
    // Filter variables for smooth Roll/Pitch
    float fXg, fYg, fZg;
    float alpha; 

    // Calculated values for output
    float value2; // Total Mag field
    float heading;
    float roll;
    float pitch;
    
    // Config state
    bool    _autoRangeEnabled;
    uint8_t _magGain;
    uint8_t _accRange;
    
    // Conversion factors
    float _lsm303Accel_MG_LSB;
    float _lsm303Mag_Gauss_LSB_XY;
    float _lsm303Mag_Gauss_LSB_Z;
};

#endif
