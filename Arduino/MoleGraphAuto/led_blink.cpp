#include "led_blink.h"

LedBlink::LedBlink(uint8_t _type, uint32_t _period, uint8_t _port) : Sensor(_type, _period, _port) {
  pin = PORTS[_port][1];
  pinMode(pin, OUTPUT);
  digitalWrite(pin, 0);
}

bool LedBlink::process() {
//  if (Millis() - time >= period) {
  if (Action(period)) {
    digitalWrite(pin, !digitalRead(pin));
    time += period;
    return 1;
  }
  return 0;
}
