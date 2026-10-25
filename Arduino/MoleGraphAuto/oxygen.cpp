#include "oxygen.h"

Oxygen::Oxygen(uint32_t _period, uint8_t _port) : Sensor(_period, _port) {
  pin       = PORTS[_port][0];
  pinMode(pin, INPUT);
  // Default calibration value measured for 21% O2
  calib_raw = 159;
}

bool Oxygen::process() {
  if (Action(period)) {
    value  = analogRead(pin);
    time += period;
    return 1;
  }
  return 0;
}

float Oxygen::read(uint8_t _spec) {
  float result = NO_DATA;
  // Prevent division by zero
  if (calib_raw == 0) return 0.0f;
  // Calculate percentage once to save flash memory (avoids duplicate float division)
  float percentage = (value * 21.0f) / (float)calib_raw;  

  switch (_spec) {
    case 0: 
        result =  percentage;            // O2 in %
      break;      
    case 1: 
         result = percentage * 10000.0f; // O2 in ppm (1% = 10000 ppm)
      break;
  }
  
  return result;
}

// Called by HW button press
void Oxygen::calibrate() {
  // Store current raw value as 21% reference
  calib_raw = analogRead(pin); 
}
