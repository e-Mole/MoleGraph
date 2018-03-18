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

bool VL53L0X::process() {
  if (Action(delta)) {
    if (active == 0) {
      I2C_WriteRegister(VL53L0X_ADDR, VL53L0X_REG_SYSRANGE_START, 0x01);
      active = 1;
      time += period; 
      delta = VL53L0X_WAIT;
  } else {
      uint8_t  val = I2C_ReadData8(VL53L0X_ADDR, VL53L0X_REG_RESULT_RANGE_STATUS);
      if ((val & 0x01) == 0) return 0;

      if ((val & 0x78) == (11 << 3)) {
        uint16_t x = I2C_ReadData16BE(VL53L0X_ADDR, VL53L0X_REG_RESULT_DISTANCE);
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

float VL53L0X::read(uint8_t spec) {
  float result = value;
  value = NO_DATA;
  return result;
}
