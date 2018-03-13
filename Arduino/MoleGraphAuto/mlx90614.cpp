#include "mlx90614.h"

#define MLX90614_ADDR  0x5A

// RAM
#define MLX90614_RAWIR1 0x04
#define MLX90614_RAWIR2 0x05
#define MLX90614_TA 0x06
#define MLX90614_TOBJ1 0x07
#define MLX90614_TOBJ2 0x08

MLX90614::MLX90614(uint32_t _period, uint8_t _port) : Sensor(_period, _port) {  
}

uint16_t read16(uint8_t a) {
  uint16_t ret;

  Wire.beginTransmission(MLX90614_ADDR); // start transmission to device 
  Wire.write(a); // sends register address to read from
  Wire.endTransmission(false); // end transmission
  
  Wire.requestFrom(MLX90614_ADDR, 3);// send data n-bytes read
  ret = Wire.read(); // receive DATA
  ret |= Wire.read() << 8; // receive DATA

  uint8_t pec = Wire.read();

  return ret;
}

bool MLX90614::process() {
  if (Action(period)) {
    time += period;
    value  = read16(MLX90614_TOBJ1);
    value2 = read16(MLX90614_TA);   
    return 1;
  } 
  return 0; 
}

float MLX90614::read(uint8_t _spec) {
  float result = NO_DATA;
  switch (_spec) {
    case 0: result = value  * 0.02 - 273.15; value  = NO_DATA; break;  // teplota objektu
    case 1: result = value2 * 0.02 - 273.15; value2 = NO_DATA; break;  // teplota okoli
    }
  return result;
}



