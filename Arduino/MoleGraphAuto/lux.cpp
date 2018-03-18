#include "lux.h"

Lux::Lux(uint32_t _period, uint8_t _port) : Sensor( _period, _port) {
  pin       = PORTS[_port][0];
  pin_digi  = PORTS[_port][1];
  pinMode(pin, INPUT);
  pinMode(pin_digi, INPUT);
}

bool Lux::process() {
  if (Action(period)) {
    uint16_t x = map(analogRead(pin), 52, 1010, 600, 0);
//    uint16_t x = analogRead(pin);
    value2 = digitalRead(pin_digi);
    value = x;
    time += period;
    return 1;
  }
  return 0;
}

float Lux::read(uint8_t _spec) {
  float result = NO_DATA;
  switch (_spec) {
    case 0: result = value;  value = NO_DATA; break;             // pseudo lux
    case 1: result = value2; value2 = NO_DATA; break;            // trigger 0/1
  }
  return result;
}
