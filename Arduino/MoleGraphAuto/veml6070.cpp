#include "veml6070.h"

VEML6070::VEML6070(uint32_t _period, uint8_t _port) : Sensor(_period, _port) {
  // Initializing in start()
}

void VEML6070::start(uint32_t now) {
  Sensor::start(now);
  initSensor();
}

void VEML6070::initSensor() {
  Wire.begin();
  
  // send command to adress 0x38
  Wire.beginTransmission(VEML6070_ADDR_L);
  Wire.write(VEML6070_CMD_1T);
  Wire.endTransmission();
}

bool VEML6070::process() {
  if (Action(period)) {
    /* Get a new sensor val */
    
    uint16_t raw_uv = readUV();
    value = (float)raw_uv;

    time += period;
    return 1;
  }
  return 0;
}

float VEML6070::read(uint8_t _spec) {
  float result = NO_DATA;
  switch (_spec) {
    case 0: result = value; break;      //RAW UV intensity
    case 1: result = value/187; break;  //UVI (Pro Rset=270k je 1 UVI cca 187 kroků)   
  }
  return result;
}

uint16_t VEML6070::readUV() {
  // Read MSB from 0x39
  if (Wire.requestFrom(VEML6070_ADDR_H, 1) != 1) return 0;
  uint16_t uvi = Wire.read();
  uvi <<= 8;

  // Read LSB from 0x38
  if (Wire.requestFrom(VEML6070_ADDR_L, 1) != 1) return 0;
  uvi |= Wire.read();

  return uvi;
}
