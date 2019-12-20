#include "amp5.h"

AMP5::AMP5(uint32_t _period, uint8_t _port) : Sensor(_period, _port) {
  pin = PORTS[_port][0];
  pinMode(pin, INPUT);
}

bool AMP5::process() {
  if (Action(period)) {
    value = analogRead(pin);
    time += period;
    return 1;
  }
  return 0;
}

float AMP5::read(uint8_t _spec) {
  //float result = value;
  //value = NO_DATA;
  float result = NO_DATA;

    // Measuring Current Using ACS712
    int mVperAmp = 185; // use 100 for 20A Module and 66 for 30A Module
    int ACSoffset = 2500;
    double Voltage = 0;
    double Amps = 0;
    Voltage = value*(5000.0f/1024); // Gets you mV
    Amps = ((Voltage - ACSoffset) / mVperAmp);

  switch (_spec) {
    case 0: result = Amps; break;       // AC/DC proud (max 5 A)
    case 1: result = value; break;      // RAW
  }
  return result;
}
