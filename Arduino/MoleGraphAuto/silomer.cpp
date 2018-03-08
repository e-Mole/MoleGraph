#include "silomer.h"

Silomer::Silomer(uint8_t _type, uint32_t _period, uint8_t _port) : Sensor(_type, _period, _port) {
  data = PORTS[_port][0];
  clk  = PORTS[_port][1];
  pinMode(data, INPUT);
  pinMode(clk, OUTPUT);
  offset = 0;
  scale = 5556.0f;
  
  digitalWrite(clk, LOW);
  getValue();
}

int32_t Silomer::getValue() {
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

bool Silomer::process() {
  if (Action(period)) {
    if (digitalRead(data)) return 0;

    value = (getValue() - offset)/ scale;
    
    time += period;
    return 1;
  }
  return 0;
}

void Silomer::calibrate() {
  offset = getValue();
}

