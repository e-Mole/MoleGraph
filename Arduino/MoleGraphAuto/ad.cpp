#include "ad.h"

AD::AD(uint8_t _type, uint32_t _period, uint8_t _port) : Sensor(_type, _period, _port) {
  pin = PORTS[_port][0];
  pinMode(pin, INPUT);
}

bool AD::process() {
  if (Action(period)) {
    value = analogRead(pin);
    time += period;
    return 1;
  }
  return 0;
}

float AD::read(uint8_t _spec) {
  float result = NO_DATA;
  switch (_spec) {
    case 0: result = value; value = NO_DATA; break;              // RAW
    case 1: result = value * 5.0f/1024; value = NO_DATA; break;  // voltage    
  }
  return result;
}

