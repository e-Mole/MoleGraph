#include "oxygen.h"

Oxygen::Oxygen(uint32_t _period, uint8_t _port) : Sensor(_period, _port) {
  pin       = PORTS[_port][0];
  pinMode(pin, INPUT);
}

bool Oxygen::process() {
  if (Action(period)) {
    value  = analogRead(pin);
    time += period;
    return 1;
  }
  return 0;
}

float Oxygen::read(uint8_t _spec) {
  float result = NO_DATA;
  switch (_spec) {
    case 0: result = value; break;        // RAW
    case 1: result = value*(5.0f/1024); break;  // ppm
    case 2: result = value*(5.0f/1024); break;  // %
  }
  return result;
}
