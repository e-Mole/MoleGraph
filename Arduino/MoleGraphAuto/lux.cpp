#include "lux.h"

Lux::Lux(uint8_t _type, uint32_t _period, uint8_t _port) : Sensor(_type, _period, _port) {
  pin = PORTS[_port][0];
  pinMode(pin, INPUT);
}

bool Lux::process() {
  if (Action(period)) {
    uint16_t x = map(analogRead(pin), 52, 1010, 600, 0);
//    uint16_t x = analogRead(pin);
    value = x;
    time += period;
    return 1;
  }
  return 0;
}
