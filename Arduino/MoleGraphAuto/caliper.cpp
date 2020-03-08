#include "caliper.h"

CALIPER::CALIPER(uint32_t _period, uint8_t _port) : Sensor(_period, _port) {
  pin_clk   = PORTS[_port][0];  //analog GPIO 02
  pin_data  = PORTS[_port][1];  //digital GPIO 01
  pinMode(pin_clk, INPUT);
  pinMode(pin_data, INPUT);
}

bool CALIPER::process() {
  if (Action(period)) {
     
	 // This code is for MoleGraph measuring with Caliper only
	  while (digitalRead(pin_clk) == LOW) {}   // If clock is LOW wait until it turns to HIGH
	  time_now = micros();
	  while (digitalRead(pin_clk) == HIGH) {}  // Wait for the end of the HIGH pulse
	  if ((micros() - time_now) > 500) {        // If the HIGH pulse was longer than 500 micros we are at the start of a new bit sequence
		decodeBits();                           // Decode the bit sequence
	  }	
    
	time += period;
    return 1;
  }
  return 0;
}

float CALIPER::read(uint8_t _spec) {
  float result = NO_DATA;

  switch (_spec) {
    case 0: result = value; break;       // mm
    case 1: result = value / 20; break;  // inch, caliper must be set to mm mode
  }
  return result;
}

void CALIPER::decodeBits() { 
  int sign = 1;
  int i = 0;
  long vtemp = 0;
  //float c = 100.00;  // 100 in mm mode, 2000 in inch mode
  
  for (i = 0; i < 23; i++) {
    while (digitalRead(pin_clk) == LOW) { } //Wait until clock returns to HIGH- the first bit is not needed
    while (digitalRead(pin_clk) == HIGH) {} //Wait until clock returns to LOW
    if (digitalRead(pin_data) == HIGH) {
      if (i < 20) {
        vtemp|= 1<<i;
      }
      if (i == 20) {  // Bit 21 is the sign bit. 0 -> +, 1 => -
        sign = -1;
      }
      /*
	  if (i == 23) {  // Bit 24 tells the measureing unit (1 -> in, 0 -> mm)
        c = 2000.00;
      }	
      */	  
    }
  }
  //value = (vtemp * sign) / c; // Result only in mm
  value = (vtemp * sign) / 100.00; 
}
