#include "mpx5700dp.h"

MPX5700DP::MPX5700DP(uint8_t _type, uint32_t _period, uint8_t _port) : Sensor(_type, _period, _port) {
  pin = PORTS[_port][0];
  pinMode(pin, INPUT);
  scale = 700000.0f/1024;
}

bool MPX5700DP::process() {
  if (Action(period)) {
    value = (analogRead(pin) - offset) * scale;
    time += period;
    return 1;
  }
  return 0;
}

void MPX5700DP::calibrate() {
  offset = analogRead(pin);
}
