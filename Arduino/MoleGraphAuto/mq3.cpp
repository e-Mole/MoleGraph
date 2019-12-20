#include "mq3.h"

MQ3::MQ3(uint32_t _period, uint8_t _port) : Sensor(_period, _port) {
  pin       = PORTS[_port][0];
  pin_digi  = PORTS[_port][1];
  pinMode(pin, INPUT);
  pinMode(pin_digi, INPUT);
}

bool MQ3::process() {
  if (Action(period)) {
    value = analogRead(pin);
    value2 = digitalRead(pin_digi);
    time += period;
    return 1;
  }
  return 0;
}

float MQ3::read(uint8_t _spec) {
  float result = NO_DATA;

  // Measuring Current Using MQ-3 Alcohol sensor (sensitive to benzine too)

    float RS_gas;
    float voltage;
    float ratio;
    float BAC;
    float R0;
    int R2 = 1000; //1000

    voltage = value*(5.0f/1024);; // Gets you V
    RS_gas = ((5.0 * R2)/voltage) - R2;
    //R0 = RS_gas / 60;	 //R0 = 110
	  R0 = 11000;
    ratio = RS_gas/R0;
    double x = 0.4 * ratio;
    BAC = pow(x,-1.431);  //BAC in mg/l = ppm

  switch (_spec) {
    case 0: result = BAC; break;          // BAC in mg/l or ppm
    case 1: result = BAC/10000; break;    // BAC in %
    case 2: result = voltage; break;  // Voltage
    case 3: result = value; break;      // RAW
    case 4: result = value2; break;    // trigger 0/1
  }
  return result;
}
