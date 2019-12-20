#include "volt25.h"

VOLT25::VOLT25(uint32_t _period, uint8_t _port) : Sensor(_period, _port) {
  pin = PORTS[_port][0];
  pinMode(pin, INPUT);
}

bool VOLT25::process() {
  if (Action(period)) {
    value = analogRead(pin);
    time += period;
    return 1;
  }
  return 0;
}

float VOLT25::read(uint8_t _spec) {
  //float result = value;
  //value = NO_DATA;
  float result = NO_DATA;
  switch (_spec) {
    case 0: result = value*(26.0f/1024); break;      // DC napeti V (max 25 V)
    case 1: result = value; break;                   // RAW
  }
  return result;
}
