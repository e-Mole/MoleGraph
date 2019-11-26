#include "soundmeter.h"

Soundmeter::Soundmeter(uint32_t _period, uint8_t _port) : Sensor(_period, _port) {
  pin       = PORTS[_port][0];
  pin_digi  = PORTS[_port][1];
  pinMode(pin, INPUT);
  pinMode(pin_digi, INPUT);
}

bool Soundmeter::process() {
  if (Action(period)) {
    value  = analogRead(pin);
    value2 = digitalRead(pin_digi);
    time += period;
    return 1;
  }
  return 0;
}

float Soundmeter::read(uint8_t _spec) {
  float result = NO_DATA;
  switch (_spec) {
    case 0: result = value; value = NO_DATA; break;        // RAW
    case 1: result = value*(5.0f/1024); value = NO_DATA; break;  // voltage
    case 2: result = value2; value2 = NO_DATA; break;            // trigger 0/1
  }
  return result;
}
