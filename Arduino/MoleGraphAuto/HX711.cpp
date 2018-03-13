#include "hx711.h"

HX711::HX711(uint32_t _period, uint8_t _port) : Sensor(_period, _port) {
  data = PORTS[_port][0];
  clk  = PORTS[_port][1];
  pinMode(data, INPUT);
  pinMode(clk, OUTPUT);
  
  digitalWrite(clk, LOW);
  getValue();
}

int32_t HX711::getValue() {
  while(digitalRead(data));

  uint32_t x = 0;
  uint8_t d[3];

  // pulse the clock pin 24 times to read the data
  d[2] = shiftIn(data, clk, MSBFIRST);
  d[1] = shiftIn(data, clk, MSBFIRST);
  d[0] = shiftIn(data, clk, MSBFIRST);

  // set the channel and the gain factor for the next reading using the clock pin
  for (uint8_t i = 0; i < gain; i++) {
    digitalWrite(clk, HIGH);
    digitalWrite(clk, LOW);
  }

  x = (uint32_t)d[2] << 16 | (uint32_t)d[1] << 8 | d[0];
  if (d[2] & 0x80) {
    x |= 0xFF000000;
  } 
  return x;  
}

bool HX711::process() {
  if (Action(period)) {
    if (digitalRead(data)) return 0;
    value = getValue();   
    time += period;
    return 1;
  }
  return 0;
}

float HX711::read(uint8_t _spec) {
  return value - offset;
}

void HX711::calibrate() {
  offset = getValue();
}


