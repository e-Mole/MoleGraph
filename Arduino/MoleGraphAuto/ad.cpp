#include "ad.h"

AD::AD(uint32_t _period, uint8_t _port) : Sensor(_period, _port) {
  pin = PORTS[_port][0];
  pin_digi  = PORTS[_port][1];  // added trigger digital input pin
  pinMode(pin, INPUT);
  pinMode(pin_digi, INPUT);    // added trigger digital input pin
}

bool AD::process() {
  if (Action(period)) {
    value = analogRead(pin);
    value2 = digitalRead(pin_digi);  // added trigger digital input pin  
    time += period;
    return 1;
  }
  return 0;
}

float AD::read(uint8_t _spec) {
  float result = NO_DATA;
  switch (_spec) {
    case 0: result = value; break; // RAW
    case 1: result = value*(5.0f/1024); break; // voltage
    case 2: result = value2; break;            // trigger 0/1   // added trigger digital input pin
  }
  return result;
}
