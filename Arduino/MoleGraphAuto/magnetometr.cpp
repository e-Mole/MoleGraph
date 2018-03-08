#include "magnetometr.h"

Magnetometr::Magnetometr(uint8_t _type, uint32_t _period, uint8_t _port) : Sensor(_type, _period, _port) {
  pin = PORTS[_port][0];
  pinMode(pin, INPUT);
}

bool Magnetometr::process() {
//  if (Millis() - time >= period) {
  if (Action(period)) {
    value = analogRead(pin);
    time += period;
    return 1;
  }
  return 0;
}
