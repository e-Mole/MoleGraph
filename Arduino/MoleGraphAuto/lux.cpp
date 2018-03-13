#include "lux.h"

Lux::Lux(uint32_t _period, uint8_t _port) : AD(_period, _port) {
}

float Lux::read() {
  float x = map(value, 52, 1010, 600, 0);
  value = NO_DATA;
  return x;
}
