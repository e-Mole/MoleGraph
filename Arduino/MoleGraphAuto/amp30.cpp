#include "amp30.h"

AMP30::AMP30(uint32_t _period, uint8_t _port) : Sensor(_period, _port) {
  pin = PORTS[_port][0];
  pinMode(pin, INPUT);
}

bool AMP30::process() {
  if (Action(period)) {
    value = analogRead(pin);
    time += period;
    return 1;
  }
  return 0;
}

float AMP30::read(uint8_t _spec) {
  //float result = value;
  //value = NO_DATA;
  float result = NO_DATA;
    
    // Measuring Current Using ACS712
    int mVperAmp = 66; // use 185 for 5A, 100 for 20A Module and 66 for 30A Module
    int ACSoffset = 2500;
    double Voltage = 0;
    double Amps = 0;
    Voltage = value*(5000.0f/1024); // Gets you mV
    Amps = ((Voltage - ACSoffset) / mVperAmp);
  
  switch (_spec) {
    case 0: result = Amps; value = NO_DATA; break;      // AC/DC proud (max 30 A)
    case 1: result = value; value = NO_DATA; break;     // RAW
  }
  return result;
}
