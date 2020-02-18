#include "max6675t.h"
// maximum sampling rate 4 Hz !

MAX6675::MAX6675(uint32_t _period, uint8_t _port) : Sensor(_period, _port) {
  pinSO   = PORTS[_port][1]; //digital GPIO 01
  pinCS   = PORTS[_port][0];   //analog GPIO 02
  pinSCK  = 10; // fixed extender must be in PORT 2 - D10 (black wire)
  
  //define pin modes
  pinMode(pinCS, OUTPUT);
  pinMode(pinSCK, OUTPUT); 
  pinMode(pinSO, INPUT);

  digitalWrite(pinCS, HIGH);
}

double MAX6675::getValue(void) {

  uint16_t v;

  digitalWrite(pinCS, LOW);
  _delay_ms(1);

  v = spiread();
  v <<= 8;
  v |= spiread();

  digitalWrite(pinCS, HIGH);

  if (v & 0x4) {
    // uh oh, no thermocouple attached!
    //return NAN; 
    return -500;
  }

  v >>= 3;

  return v*0.25;  
}
byte MAX6675::spiread(void) { 
  int i;
  byte d = 0;

  for (i=7; i>=0; i--)
  {
    digitalWrite(pinSCK, LOW);
    _delay_ms(1);
    if (digitalRead(pinSO)) {
      //set the bit to 0 no matter what
      d |= (1 << i);
    }

    digitalWrite(pinSCK, HIGH);
    _delay_ms(1);
  }

  return d;
}

bool MAX6675::process() {
  if (Action(period)) {
    value = getValue();   
    time += period;
    return 1;
  }
  return 0;
}

float MAX6675::read(uint8_t _spec) {
  return value - offset;
}

void MAX6675::calibrate() {
  offset = getValue();
}
