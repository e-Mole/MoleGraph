#include "mpx5700dp.h"

MPX5700DP::MPX5700DP(uint32_t _period, uint8_t _port) : AD(_period, _port) {
  scale = 700000.0f/1024;
}

float MPX5700DP::read(uint8_t _spec) {
  return (value - offset) * scale;
}

void MPX5700DP::calibrate() {
  offset = analogRead(pin);
}
