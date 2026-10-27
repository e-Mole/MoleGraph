#ifndef TCS34725_SPECTRO_h
#define TCS34725_SPECTRO_h

#include "timer.h"

// --- I2C Address and Registers for TCS34725 ---
#define TCS34725_ADDRESS          0x29
#define TCS34725_COMMAND_BIT      0x80
#define TCS34725_ENABLE           0x00
#define TCS34725_ENABLE_PON       0x01 // Power ON
#define TCS34725_ENABLE_AEN       0x02 // ADC Enable
#define TCS34725_ATIME            0x01 // Integration time register
#define TCS34725_CONTROL          0x0F // Gain control register

class TCS34725_Spectro : public Sensor {
  public:
    TCS34725_Spectro(uint32_t _period, uint8_t _port);
    virtual bool process();
    virtual float read(uint8_t _spec);
    virtual void calibrate(); 

  private:
    // Raw current data from sensor
    uint16_t raw_r, raw_g, raw_b, raw_c;
    
    // Calibration reference - Step 1 (Light / Blank)
    uint16_t light_r, light_g, light_b, light_c;
    
    // Calibration reference - Step 2 (Dark / Noise)
    uint16_t dark_r, dark_g, dark_b, dark_c;

    uint8_t _i2caddr;
    uint8_t led_pin;

    // Variables for the calibration state machine
    uint8_t calib_state; // 0 = waiting for light, 1 = waiting for dark
    uint32_t last_calib_time;
    uint32_t last_process_time; // Shield against MoleGraph core 100ms spam bug  
    bool led_forced_off;
    
    bool initSensor();
    void readData();
    float calcT(uint16_t raw, uint16_t light, uint16_t dark); // Helper function to calculate transmittance
};

#endif
