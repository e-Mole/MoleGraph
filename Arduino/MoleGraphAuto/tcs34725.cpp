#include "tcs34725.h"

#ifdef PROFILE_MONITOR
#include <avr/pgmspace.h>
// ====================================================================
// MONITOR PROFILE LUT TABLES (PROGMEM)
// ====================================================================
const uint16_t hueLUT[361] PROGMEM = {
      9,  13,  17,  20,  24,  28,  30,  31,  32,  33,  34,  35,  36,  36,  37,
     38,  39,  40,  41,  42,  42,  43,  44,  45,  46,  47,  48,  48,  49,  50,
     51,  52,  53,  54,  54,  55,  56,  57,  58,  59,  60,  60,  61,  61,  61,
     62,  62,  63,  63,  63,  64,  64,  65,  65,  65,  66,  66,  67,  67,  67,
     68,  68,  69,  69,  69,  70,  70,  71,  71,  71,  72,  72,  72,  73,  73,
     74,  74,  74,  75,  75,  76,  76,  76,  77,  77,  78,  78,  78,  79,  79,
     80,  80,  80,  81,  81,  82,  82,  82,  83,  83,  84,  84,  84,  85,  85,
     86,  86,  86,  87,  87,  87,  88,  88,  89,  89,  89,  90,  91,  93,  94,
     96,  97,  99, 100, 102, 103, 105, 107, 108, 110, 111, 113, 114, 116, 117,
    119, 121, 123, 126, 128, 130, 133, 135, 137, 140, 142, 145, 147, 149, 151,
    152, 153, 154, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165,
    166, 167, 168, 169, 170, 171, 171, 172, 173, 174, 175, 176, 177, 178, 179,
    180, 181, 182, 183, 184, 185, 185, 186, 187, 188, 189, 190, 191, 192, 193,
    194, 195, 196, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207,
    207, 208, 209, 210, 213, 215, 218, 220, 222, 225, 227, 229, 232, 234, 236,
    239, 241, 242, 244, 246, 247, 249, 250, 252, 253, 255, 257, 258, 260, 261,
    263, 264, 266, 268, 269, 270, 271, 271, 271, 272, 272, 273, 273, 274, 274,
    274, 275, 275, 276, 276, 276, 277, 277, 278, 278, 279, 279, 279, 280, 280,
    281, 281, 281, 282, 282, 283, 283, 283, 284, 284, 285, 285, 286, 286, 286,
    287, 287, 288, 288, 288, 289, 289, 290, 290, 291, 291, 291, 292, 292, 293,
    293, 293, 294, 294, 295, 295, 296, 296, 296, 297, 297, 298, 298, 298, 299,
    299, 300, 300, 301, 302, 303, 304, 305, 306, 307, 307, 308, 309, 310, 311,
    312, 313, 314, 315, 316, 316, 317, 318, 319, 320, 321, 322, 323, 324, 325,
    325, 326, 327, 328, 329, 330, 334, 338, 342, 346, 350, 354, 358,   0,   1,
      2
};

const uint8_t gamma100[256] PROGMEM = {
      0,   8,  11,  13,  15,  17,  18,  20,  21,  22,  23,  24,  25,  26,  27,  28,
     28,  29,  30,  31,  31,  32,  33,  34,  34,  35,  35,  36,  37,  37,  38,  38,
     39,  39,  40,  41,  41,  42,  42,  43,  43,  44,  44,  45,  45,  45,  46,  46,
     47,  47,  48,  48,  49,  49,  49,  50,  50,  51,  51,  51,  52,  52,  53,  53,
     53,  54,  54,  54,  55,  55,  56,  56,  56,  57,  57,  57,  58,  58,  58,  59,
     59,  59,  60,  60,  60,  61,  61,  61,  62,  62,  62,  63,  63,  63,  64,  64,
     64,  64,  65,  65,  65,  66,  66,  66,  67,  67,  67,  67,  68,  68,  68,  69,
     69,  69,  69,  70,  70,  70,  70,  71,  71,  71,  72,  72,  72,  72,  73,  73,
     73,  73,  74,  74,  74,  74,  75,  75,  75,  75,  76,  76,  76,  76,  77,  77,
     77,  77,  78,  78,  78,  78,  79,  79,  79,  79,  80,  80,  80,  80,  80,  81,
     81,  81,  81,  82,  82,  82,  82,  82,  83,  83,  83,  83,  84,  84,  84,  84,
     84,  85,  85,  85,  85,  86,  86,  86,  86,  86,  87,  87,  87,  87,  87,  88,
     88,  88,  88,  89,  89,  89,  89,  89,  90,  90,  90,  90,  90,  91,  91,  91,
     91,  91,  92,  92,  92,  92,  92,  93,  93,  93,  93,  93,  94,  94,  94,  94,
     94,  94,  95,  95,  95,  95,  95,  96,  96,  96,  96,  96,  97,  97,  97,  97,
     97,  97,  98,  98,  98,  98,  98,  99,  99,  99,  99,  99,  99, 100, 100, 100
};

float applyHueLUT(float raw_hue) {
    int index = (int)(raw_hue + 0.5f);
    if (index > 360) index = 360;
    if (index < 0) index = 0;
    return (float)pgm_read_word(&hueLUT[index]);
}
#endif

// ====================================================================
// COMMON FUNCTIONS
// ====================================================================

TCS34725::TCS34725(uint32_t _period, uint8_t _port) : Sensor(_period, _port) {
  _i2caddr = TCS34725_ADDRESS;
  
  hue_deg = 0.0;
  sat_pct = 0.0;
  light_pct = 0.0;

  r_coeff = 1.0f;
  g_coeff = 1.0f;
  b_coeff = 1.0f;
  
  #ifdef PROFILE_MONITOR
  cmax_reference = 255.0f; 
  cmin_reference = 0.0f;
  prev_cmax_reference = 255.0f;
  prev_cmin_reference = 0.0f;
  last_calib_time = 0;
  calib_state = 0; 
  #else
  cmax_reference = 10240.0f; // Default physical max[cite: 3]
  prev_cmax_reference = 10240.0f;
  #endif

  last_click_time = 0;
  last_process_time = 0;

  led_pin = PORTS[_port][1]; 
  pinMode(led_pin, OUTPUT);
  
  led_state = TCS34725_LED_DEFAULT_ON;
  digitalWrite(led_pin, led_state ? HIGH : LOW);

  initSensor();
}

bool TCS34725::process() {
  if (Action(period)) {
    readData();
    time += period;
    return 1;
  }
  return 0;
}

float TCS34725::read(uint8_t _spec) {
  float result = NO_DATA;
  switch (_spec) {
    case 0: result = hue_deg; break;    
    case 1: result = sat_pct; break;    
    case 2: result = light_pct; break;  
  }
  return result;
}

bool TCS34725::initSensor() {
  Wire.begin();
  
  I2C_WriteRegister(_i2caddr, TCS34725_COMMAND_BIT | TCS34725_ENABLE, TCS34725_ENABLE_PON);
  
  // Use custom delay_us() because standard Arduino delay() is broken by MoleGraph Timer0 override[cite: 3]
  delay_us(3000); 
  
  I2C_WriteRegister(_i2caddr, TCS34725_COMMAND_BIT | TCS34725_ENABLE, TCS34725_ENABLE_PON | TCS34725_ENABLE_AEN);
  
  #ifdef PROFILE_MONITOR
  I2C_WriteRegister(_i2caddr, TCS34725_COMMAND_BIT | TCS34725_ATIME, 0xD5); // 101 ms
  I2C_WriteRegister(_i2caddr, TCS34725_COMMAND_BIT | TCS34725_CONTROL, 0x03); // 60x Gain
  #else
  I2C_WriteRegister(_i2caddr, TCS34725_COMMAND_BIT | TCS34725_ATIME, 0xF6); // 24 ms[cite: 3]
  I2C_WriteRegister(_i2caddr, TCS34725_COMMAND_BIT | TCS34725_CONTROL, 0x02); // 16x Gain[cite: 3]
  #endif

  return true;
}

// Called by MoleGraph core when the hardware button is pressed
void TCS34725::calibrate() {
  // Use MoleGraph's custom Millis() because standard Arduino millis() is killed by Timer0 override[cite: 3]
  uint32_t now = Millis();
  
  // --- SPAM SHIELD ---
  // MoleGraph loop() calls this method thousands of times during a single 100ms button window.[cite: 3]
  // We ignore all calls that happen within 200ms of the previous one.[cite: 3]
  if (now - last_process_time < 200) {
      return;
  }
  last_process_time = now;
  // -------------------

  // If less than 600 ms passed since the actual last click, it's a DOUBLE CLICK[cite: 3]
  if (last_click_time != 0 && (now - last_click_time < 600)) {
    // 1. Toggle the LED state[cite: 3]
    led_state = !led_state;
    digitalWrite(led_pin, led_state ? HIGH : LOW);
    
    // 2. UNDO trick: Revert calibration coefficients to previous state[cite: 3]
    r_coeff = prev_r_coeff;
    g_coeff = prev_g_coeff;
    b_coeff = prev_b_coeff;
    cmax_reference = prev_cmax_reference; // Restore brightness reference[cite: 3]
    
    #ifdef PROFILE_MONITOR
    cmin_reference = prev_cmin_reference;
    calib_state = 0; // Cancel multi-step calibration
    #endif

    last_click_time = 0; // Reset to prevent a third click acting as another double click[cite: 3]
  } 
  // SINGLE CLICK (or the first click of a potential double click)[cite: 3]
  else {
    performCalibration();
    last_click_time = now;
  }
}

// ====================================================================
// PROFILE SPECIFIC FUNCTIONS
// ====================================================================

#ifdef PROFILE_MONITOR

void TCS34725::performCalibration() {
  if (calib_state == 0) {
    prev_r_coeff = r_coeff;
    prev_g_coeff = g_coeff;
    prev_b_coeff = b_coeff;
    prev_cmax_reference = cmax_reference;
    prev_cmin_reference = cmin_reference;
  }

  uint16_t raw_r = I2C_ReadData16LE(_i2caddr, TCS34725_COMMAND_BIT | 0x16); 
  uint16_t raw_g = I2C_ReadData16LE(_i2caddr, TCS34725_COMMAND_BIT | 0x18); 
  uint16_t raw_b = I2C_ReadData16LE(_i2caddr, TCS34725_COMMAND_BIT | 0x1A); 

  if (raw_r == 0) raw_r = 1;
  if (raw_g == 0) raw_g = 1;
  if (raw_b == 0) raw_b = 1;

  uint16_t max_val = max(raw_r, max(raw_g, raw_b));
  uint32_t now = Millis();
  
  if (now - last_calib_time > 10000) {
      calib_state = 0;
  }
  last_calib_time = now;

  if (calib_state == 0) {
    cmax_reference = (float)max_val;
    r_coeff = cmax_reference / raw_r;
    g_coeff = cmax_reference / raw_g;
    b_coeff = cmax_reference / raw_b;
    cmin_reference = 0.0f; 
    calib_state = 1; 
  } 
  else {
    cmin_reference = (float)max_val;
    calib_state = 0; 
  }
}

void TCS34725::readData() {
  uint16_t raw_c = I2C_ReadData16LE(_i2caddr, TCS34725_COMMAND_BIT | 0x14); 
  uint16_t raw_r = I2C_ReadData16LE(_i2caddr, TCS34725_COMMAND_BIT | 0x16); 
  uint16_t raw_g = I2C_ReadData16LE(_i2caddr, TCS34725_COMMAND_BIT | 0x18); 
  uint16_t raw_b = I2C_ReadData16LE(_i2caddr, TCS34725_COMMAND_BIT | 0x1A); 

  if (raw_c == 0) {
    hue_deg = 0; sat_pct = 0; light_pct = 2.0f;
    return;
  }

  float r = raw_r * r_coeff;
  float g = raw_g * g_coeff;
  float b = raw_b * b_coeff;

  float cmax = max(r, max(g, b));
  float cmin = min(r, min(g, b));
  float delta = cmax - cmin;

  float effective_cmax = cmax - cmin_reference;
  if (effective_cmax < 0.0f) effective_cmax = 0.0f;
  
  float effective_range = cmax_reference - cmin_reference;
  if (effective_range <= 0.0f) effective_range = 1.0f;

  float v_norm = (effective_cmax / effective_range) * 255.0f;
  if (v_norm > 255.0f) v_norm = 255.0f;
  
  light_pct = pgm_read_byte(&gamma100[(uint8_t)(v_norm + 0.5f)]);

  if (light_pct < 2.0f) {
      light_pct = 2.0f;
  }

  if (cmax > 0.001f) {
    sat_pct = (delta / cmax) * 150.0f; 
    if (sat_pct > 100.0f) sat_pct = 100.0f;
  } else {
    sat_pct = 0.0f;
  }

  float h = 0.0f;
  if (delta > 0.001f) { 
    if (cmax == r) {
      h = 60.0f * ((g - b) / delta);
    } else if (cmax == g) {
      h = 60.0f * (((b - r) / delta) + 2.0f);
    } else if (cmax == b) {
      h = 60.0f * (((r - g) / delta) + 4.0f);
    }
    if (h < 0.0f) h += 360.0f;
    if (h >= 360.0f) h -= 360.0f;
  }
  
  hue_deg = applyHueLUT(h); 
}

#else
// ====================================================================
// BASIC PROFILE (ORIGINAL CODE FOR PRINT/PAPER)
// ====================================================================

void TCS34725::performCalibration() {
  // Save current coefficients to backup before applying new ones[cite: 3]
  prev_r_coeff = r_coeff;
  prev_g_coeff = g_coeff;
  prev_b_coeff = b_coeff;
  prev_cmax_reference = cmax_reference;

  uint16_t raw_c = I2C_ReadData16LE(_i2caddr, TCS34725_COMMAND_BIT | 0x14); // Read Clear[cite: 3]
  uint16_t raw_r = I2C_ReadData16LE(_i2caddr, TCS34725_COMMAND_BIT | 0x16); 
  uint16_t raw_g = I2C_ReadData16LE(_i2caddr, TCS34725_COMMAND_BIT | 0x18); 
  uint16_t raw_b = I2C_ReadData16LE(_i2caddr, TCS34725_COMMAND_BIT | 0x1A); 

  if (raw_r == 0) raw_r = 1;
  if (raw_g == 0) raw_g = 1;
  if (raw_b == 0) raw_b = 1;
  if (raw_c == 0) raw_c = 1;

  cmax_reference = (float)raw_c; // Set current illumination as 100%[cite: 3]

  uint16_t max_val = max(raw_r, max(raw_g, raw_b));

  r_coeff = (float)max_val / raw_r;
  g_coeff = (float)max_val / raw_g;
  b_coeff = (float)max_val / raw_b;
}

void TCS34725::readData() {
  uint16_t raw_c = I2C_ReadData16LE(_i2caddr, TCS34725_COMMAND_BIT | 0x14); 
  uint16_t raw_r = I2C_ReadData16LE(_i2caddr, TCS34725_COMMAND_BIT | 0x16); 
  uint16_t raw_g = I2C_ReadData16LE(_i2caddr, TCS34725_COMMAND_BIT | 0x18); 
  uint16_t raw_b = I2C_ReadData16LE(_i2caddr, TCS34725_COMMAND_BIT | 0x1A); 

  if (raw_c == 0) {
    hue_deg = 0; sat_pct = 0; light_pct = 0;
    return; 
  }

  // Apply white balance coefficients[cite: 3]
  float r = raw_r * r_coeff;
  float g = raw_g * g_coeff;
  float b = raw_b * b_coeff;

  // Calculate brightness relative to calibrated reference[cite: 3]
  float v = (raw_c / cmax_reference) * 100.0f; 
  if (v > 100.0f) v = 100.0f;

  float cmax = max(r, max(g, b));
  float cmin = min(r, min(g, b));
  float delta = cmax - cmin;

  float h = 0.0f;
  float s = 0.0f;

  if (cmax > 0.0f) {
    s = delta / cmax;
  }

  if (delta > 0.00001f) { 
    if (cmax == r) {
      h = 60.0f * ((g - b) / delta);
    } else if (cmax == g) {
      h = 60.0f * (((b - r) / delta) + 2.0f);
    } else if (cmax == b) {
      h = 60.0f * (((r - g) / delta) + 4.0f);
    }
    if (h < 0.0f) h += 360.0f;
  }

  hue_deg = h;
  sat_pct = s * 100.0f;
  light_pct = v;
}
#endif
