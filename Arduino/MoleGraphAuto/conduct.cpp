#include "conduct.h"

CONDUCT::CONDUCT(uint32_t _period, uint8_t _port) : AD(_period, _port) {
  pin = PORTS[_port][0];
  pin_digi  = PORTS[_port][1];  // added trigger digital input pin
  pinMode(pin, INPUT);
  pinMode(pin_digi, INPUT);    // added trigger digital input pin  
}

float CONDUCT::read(uint8_t _spec) {
  float result = NO_DATA;
  switch (_spec) {
    case 0: result = value; break; // conductivity (µS/cm) - calibration in JSON
    case 1: result = value; break; // salinity (ppm) - calibration in JSON
    case 2: result = value; break; // RAW
    case 3: result = value*(5.0f/1024); break; // voltage
    case 4: result = value2; break;            // trigger 0/1   // added trigger digital input pin, not present on TDS module (!)
  }
  return result;
}
