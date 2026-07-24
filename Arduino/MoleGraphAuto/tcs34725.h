#ifndef TCS34725_h
#define TCS34725_h

#include "timer.h"

// ====================================================================
// FIRMWARE PROFILE SWITCH
// Uncomment exactly ONE profile below to optimize Arduino NANO memory.
// ====================================================================
//#define PROFILE_MONITOR 1 // Advanced LUTs, sRGB Gamma, 2-step calibration for emissive screens
/*
 * --- MONITOR PROFILE CALIBRATION & CONTROL GUIDE ---
 * 1. LED Toggle & Undo (Double Click):
 *    - Double-click the button (two clicks within 600ms) to toggle the built-in LED state (ON/OFF) 
 *      and simultaneously trigger an UNDO action, reverting calibration coefficients to the previous state.* 
 *      
 * 2. 2-Step Calibration (Single Click on Button):
 *    - Attention! This advanced color sensor mode requires more memory and the Arduino NANO does not have it.
 *      It is necessary to get memory by "commenting" certain sensors in the core.cpp file. We recommend commenting out sensors that you do not use often.
 *    - Step 1: Place the sensor on a White area on the screen and single-click. This sets the white reference (cmax).
 *    - Step 2: Place the sensor on a Black area and single-click within 10 seconds. This sets the black point (cmin) to compensate for backlight bleed / IPS glow.
 *    - Note: If more than 10 seconds pass before the second click, the state resets back to Step 1 (White).
 *   
 */
#define PROFILE_BASIC 1   // Lightweight code (Monitor/Paper), HW Button - LED Toggle (Double Click) and White calibration (Single Click)

// --- I2C Address and Registers for TCS34725 ---
#define TCS34725_ADDRESS          0x29
#define TCS34725_COMMAND_BIT      0x80
#define TCS34725_ENABLE           0x00
#define TCS34725_ENABLE_PON       0x01 // Power ON
#define TCS34725_ENABLE_AEN       0x02 // ADC Enable
#define TCS34725_ATIME            0x01 // Integration time register
#define TCS34725_CONTROL          0x0F // Gain control register
#define TCS34725_CDATAL           0x14 // Clear channel data low byte

/* 
 * --- Integration Time (ATIME) Options ---
 * Sets the internal integration time of the sensor.
 * Longer times provide higher resolution and better low-light performance, 
 * but decrease the maximum sampling rate.
 * 
 * 0xFF = 2.4 ms  (Fastest, lowest resolution)
 * 0xF6 = 24 ms   (Good balance for real-time tracking)
 * 0xD5 = 101 ms
 * 0xC0 = 154 ms
 * 0x00 = 700 ms  (Slowest, highest resolution)
 * 
 * --- Gain (CONTROL) Options ---
 * Sets the sensitivity of the sensor.
 * 
 * 0x00 = 1x Gain
 * 0x01 = 4x Gain   (Good default for normal lighting)
 * 0x02 = 16x Gain  (Good for darker environments)
 * 0x03 = 60x Gain  (For very low light conditions)
 */

/* 
 * --- Default LED State Macro ---
 * Set to 1 to boot up with LED ON.
 * Set to 0 to boot up with LED OFF.
 * Can be toggled at runtime using a double-click on the hardware button.
 */
#define TCS34725_LED_DEFAULT_ON 1

class TCS34725 : public Sensor {
  public:
    TCS34725(uint32_t _period, uint8_t _port);
    virtual bool process();
    virtual float read(uint8_t _spec);
    virtual void calibrate(); // Called by the core framework on hardware button press

  private:
    float hue_deg;
    float sat_pct;
    float light_pct; // Used for Value (Brightness) in HSV model

    // White balance coefficients
    float r_coeff;
    float g_coeff;
    float b_coeff; 

   // Backup for "Undo" logick (doubleclick)
    float prev_r_coeff;
    float prev_g_coeff;
    float prev_b_coeff;
    float prev_cmax_reference; // Backup for undo logic[cite: 4]

    float cmax_reference;      // Reference for 100% brightness[cite: 4]

    #ifdef PROFILE_MONITOR
    float cmin_reference;      // Reference for 0% brightness (Black point)
    float prev_cmin_reference; 
    uint32_t last_calib_time;
    uint8_t calib_state;       // 0 = white, 1 = black
    #endif

    uint8_t _i2caddr;
    
    // LED and button variables
    bool led_state;
    uint32_t last_click_time;
    uint32_t last_process_time; // Shield against MoleGraph core 100ms spam bug    
    uint8_t led_pin; // Variable for the LED control pin    

    bool initSensor();
    void readData();
    void performCalibration(); // The actual calibration logic
};

#endif
