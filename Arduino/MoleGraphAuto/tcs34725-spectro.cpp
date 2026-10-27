#include "core.h"
#include "tcs34725-spectro.h"

TCS34725_Spectro::TCS34725_Spectro(uint32_t _period, uint8_t _port) : Sensor(_period, _port) {
  _i2caddr = TCS34725_ADDRESS;
  
  // Clear raw and dark data
  raw_r = 0; raw_g = 0; raw_b = 0; raw_c = 0;
  dark_r = 0; dark_g = 0; dark_b = 0; dark_c = 0;
  
  // Default "light" must be > 0 to prevent division by zero before user calibration, use max
  light_r = 65535; light_g = 65535; light_b = 65535; light_c = 65535;

  calib_state = 0;
  last_calib_time = 0;
  last_process_time = 0;

  // Sensor LED is forced OFF - external light source is expected for spectrophotometry
  led_forced_off = false; // Nastavení pojistky na false
  // Pin si jen uložíme, ale zhasínat ho budeme až za běhu v process()
  led_pin = PORTS[_port][0];
  pinMode(led_pin, OUTPUT);
  
  initSensor();
}

bool TCS34725_Spectro::initSensor() {
  Wire.begin();
  
  I2C_WriteRegister(_i2caddr, TCS34725_COMMAND_BIT | TCS34725_ENABLE, TCS34725_ENABLE_PON);
  delay_us(3000); // Required due to MoleGraph Timer0 override
  I2C_WriteRegister(_i2caddr, TCS34725_COMMAND_BIT | TCS34725_ENABLE, TCS34725_ENABLE_PON | TCS34725_ENABLE_AEN);
  
  // DEFAULT SETTINGS FOR SPECTROPHOTOMETRY (Adjust as needed)
  // 0xD5 = 101 ms integration time (Good balance between stability and speed)
  I2C_WriteRegister(_i2caddr, TCS34725_COMMAND_BIT | TCS34725_ATIME, 0xD5); 
  // 0x02 = 16x Gain (Higher sensitivity needed for light passing through a solution)
  // 0x01 = 4x Gain, 0x03 = 60x Gain
  I2C_WriteRegister(_i2caddr, TCS34725_COMMAND_BIT | TCS34725_CONTROL, 0x02); 

  return true;
}

bool TCS34725_Spectro::process() {
  if (!led_forced_off) {
    digitalWrite(led_pin, LOW);
    led_forced_off = true;
  }
  
  if (Action(period)) {
    readData();
    time += period;
    return 1;
  }
  return 0;
}

void TCS34725_Spectro::readData() {
  raw_c = I2C_ReadData16LE(_i2caddr, TCS34725_COMMAND_BIT | 0x14); 
  raw_r = I2C_ReadData16LE(_i2caddr, TCS34725_COMMAND_BIT | 0x16); 
  raw_g = I2C_ReadData16LE(_i2caddr, TCS34725_COMMAND_BIT | 0x18); 
  raw_b = I2C_ReadData16LE(_i2caddr, TCS34725_COMMAND_BIT | 0x1A); 
}

void TCS34725_Spectro::calibrate() {
  // Use MoleGraph's custom Millis() due to Timer0 override
  uint32_t now = Millis();
  
  // --- SPAM SHIELD ---
  // Prevent button bouncing and core spamming.
  // 500ms is a safe window for a standard human single-click.
  if (now - last_process_time < 500) {
      return;
  }
  last_process_time = now;

  // Timeout reset: if the second click doesn't happen within 30 seconds, reset to step 1
  if (calib_state == 1 && (now - last_calib_time > 30000)) {
      calib_state = 0;
  }
  last_calib_time = now;

  // --- EXPLICIT I2C READ ---
  // Crucial fix: We must fetch completely fresh data directly from the sensor 
  // exactly at the moment of the button press, bypassing the background loop.
  uint16_t current_c = I2C_ReadData16LE(_i2caddr, TCS34725_COMMAND_BIT | 0x14); 
  uint16_t current_r = I2C_ReadData16LE(_i2caddr, TCS34725_COMMAND_BIT | 0x16); 
  uint16_t current_g = I2C_ReadData16LE(_i2caddr, TCS34725_COMMAND_BIT | 0x18); 
  uint16_t current_b = I2C_ReadData16LE(_i2caddr, TCS34725_COMMAND_BIT | 0x1A); 

  if (calib_state == 0) {
    // 1st CLICK - Save Light (Blank) reference
    light_c = current_c; 
    light_r = current_r; 
    light_g = current_g; 
    light_b = current_b;
    
    // Reset dark to zero just in case the user skips the second step
    dark_c = 0; dark_r = 0; dark_g = 0; dark_b = 0; 
    
    calib_state = 1; 

    DEBUG_MSG("LIGHT")
    
    // LED FEEDBACK: red bliknutí
    digitalWrite(2, !digitalRead(2));
    delay_us(1000000); 
    digitalWrite(2, !digitalRead(2));    
    
  } else {
    // 2nd CLICK - Save Dark reference
    dark_c = current_c; 
    dark_r = current_r; 
    dark_g = current_g; 
    dark_b = current_b;
    
    calib_state = 0; 

    DEBUG_MSG("DARK")

    // LED FEEDBACK: green bliknutí
    digitalWrite(3, !digitalRead(3));
    delay_us(1000000); 
    digitalWrite(3, !digitalRead(3));
  }
}

// Memory-efficient helper to calculate transmittance in percentage
float TCS34725_Spectro::calcT(uint16_t raw, uint16_t light, uint16_t dark) {
  if (light <= dark) return 100.0f; // Division by zero protection
  if (raw <= dark) return 0.0f;     // Absolute darkness protection
  
  // Každou proměnnou natvrdo převedeme na float ještě před matematikou
  float f_raw = (float)raw;
  float f_light = (float)light;
  float f_dark = (float)dark;
  
  float t = ((f_raw - f_dark) / (f_light - f_dark)) * 100.0f;
  
  //if (t > 100.0f) return 100.0f;    // Noise cap
  return t;
}

float TCS34725_Spectro::read(uint8_t _spec) {
  float result = NO_DATA;
  switch (_spec) {
    // Transmittance (0-100%)
    case 0: result = calcT(raw_r, light_r, dark_r); break;
    case 1: result = calcT(raw_g, light_g, dark_g); break;
    case 2: result = calcT(raw_b, light_b, dark_b); break;
    case 3: result = calcT(raw_c, light_c, dark_c); break;
    
    // Raw unprocessed data for manual student analysis
    case 4: result = raw_r; break; 
    case 5: result = raw_g; break; 
    case 6: result = raw_b; break; 
    case 7: result = raw_c; break; 
  }
  return result;
}
