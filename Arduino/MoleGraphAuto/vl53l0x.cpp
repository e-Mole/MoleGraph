#include "vl53l0x.h"

#define VL53L0X_WAIT  55
#define VL53L0X_ADDR  (0x52 >> 1)

#define VL53L0X_REG_IDENTIFICATION_MODEL_ID         0xc0
#define VL53L0X_REG_IDENTIFICATION_REVISION_ID      0xc2
#define VL53L0X_REG_PRE_RANGE_CONFIG_VCSEL_PERIOD   0x50
#define VL53L0X_REG_FINAL_RANGE_CONFIG_VCSEL_PERIOD 0x70
#define VL53L0X_REG_SYSRANGE_START                  0x00
#define VL53L0X_REG_RESULT_INTERRUPT_STATUS         0x13
#define VL53L0X_REG_RESULT_RANGE_STATUS             0x14
#define VL53L0X_REG_RESULT_DISTANCE                 0x1E

VL53L0X::VL53L0X(uint32_t _period, uint8_t _port) : Sensor(_period, _port) {
  delta  = period;
  active = 0;
}

void write_byte_data(uint8_t data) {
  Wire.beginTransmission(VL53L0X_ADDR);
  Wire.write(data);
  Wire.endTransmission();
}

void write_byte_data_at(uint8_t reg, uint8_t data) {
  Wire.beginTransmission(VL53L0X_ADDR);
  Wire.write(reg);
  Wire.write(data);
  Wire.endTransmission();
}

uint8_t read_byte_data_at(uint8_t reg) {
  write_byte_data(reg);
  Wire.requestFrom(VL53L0X_ADDR, 1);
  while (Wire.available() < 1);
  uint8_t b = Wire.read();
  return b;
}

uint16_t read_word_data_at(uint8_t reg) {
  uint8_t d[2];
  write_byte_data(reg);
  Wire.requestFrom(VL53L0X_ADDR, 2);
  while (Wire.available() < 2);
  d[0] = Wire.read();
  d[1] = Wire.read();
  return (uint16_t)d[0] << 8 | d[1]; 
}

bool VL53L0X::process() {
  if (Action(delta)) {
    if (active == 0) {
      write_byte_data_at(VL53L0X_REG_SYSRANGE_START, 0x01);
      active = 1;
      time += period;
      delta = VL53L0X_WAIT;
  } else {
      uint8_t  val = read_byte_data_at(VL53L0X_REG_RESULT_RANGE_STATUS);
//Serial.println(val, HEX);
      if ((val & 0x01) == 0) return 0;

      if ((val & 0x78) == (11 << 3)) {
        uint16_t x = read_word_data_at(VL53L0X_REG_RESULT_DISTANCE);
        value = x * (0.001f);
      } else {
        value = NO_DATA;
      }
      active = 0;
      delta  = period;
      return 1;
    }
  }
  return 0;
}
