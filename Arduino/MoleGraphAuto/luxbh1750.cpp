#include "LUXBH1750.h"

LUXBH1750::LUXBH1750(uint32_t _period, uint8_t _port) : Sensor(_period, _port) {
    Wire.begin();
    
    // High Resolution continual measurement - BH1750_CONTINUOUS_HIGH_RES_MODE)
    configure(BH1750_CONTINUOUS_HIGH_RES_MODE);
    // Low Resolution continual measurement - BH1750_CONTINUOUS_LOW_RES_MODE)
    //configure(BH1750_CONTINUOUS_LOW_RES_MODE);    
}

bool LUXBH1750::process() {
  if (Action(period)) {
    // Read value
    uint16_t level = readLightLevel();
    value = (float)level / 1.2;

    time += period;
    return 1;
  }
  return 0;
}

float LUXBH1750::read(uint8_t _spec) {
  float result = NO_DATA;
  switch (_spec) {
    case 0: result = value; break; //Light intensity - lux
  }
  return result;
}

void LUXBH1750::configure(uint8_t mode) {
    // I2C communication
    Wire.beginTransmission(BH1750_I2CADDR);
    Wire.write(mode);
    Wire.endTransmission();
}

uint16_t LUXBH1750::readLightLevel() {
    uint16_t level = 0;

    Wire.beginTransmission(BH1750_I2CADDR);
    Wire.requestFrom(BH1750_I2CADDR, 2); // need 2 byte only

    if (Wire.available() == 2) {
        level = Wire.read();
        level <<= 8;
        level |= Wire.read();
    }
    
    return level;
}
