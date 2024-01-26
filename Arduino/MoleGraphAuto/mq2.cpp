#include "mq2.h"

MQ2::MQ2(uint32_t _period, uint8_t _port) : Sensor(_period, _port) {
  pin       = PORTS[_port][0];
  pin_digi  = PORTS[_port][1];
  pinMode(pin, INPUT);
  pinMode(pin_digi, INPUT);
  offset = 0; 
}

bool MQ2::process() {
  if (Action(period)) {
    value = analogRead(pin);
    value2 = digitalRead(pin_digi);
    time += period;
    return 1;
  }
  return 0;
}

float MQ2::read(uint8_t _spec) {
  float result = NO_DATA;
/*
  // Measuring Using MQ-3 Alcohol sensor (sensitive to benzine too)
    #define R2  10.0     //value of resistor RL, typical 47K, 10K, 2K, 1K (this value must be determined form sensor pcb)
    #define m  -0.840    //calculated Slope
    #define b   0.605    //calculated Intercept
    #define R0  133.0    //precalculate (found) R0 value (in fresh air conditions), //R0 = 60, 130, 1, 0.6, 0.17
*/
  // Measuring Using MQ-2 LPG, methan, hydrogen
    #define R2  10.0     //value of resistor RL, typical 47K, 10K, 2K, 1K (this value must be determined form sensor pcb)
    #define m  -0.670    //calculated Slope
    #define b   1.071    //calculated Intercept
    #define R0  49.0     //precalculate (found) R0 value (in fresh air conditions), //R0 = 60, 130, 1, 0.6, 0.17    

    float RS_gas;
    float voltage;
    float ratio;
    float BAC;

    voltage = (value - offset)*(5.0f/1024); // Gets you V
    RS_gas = ((5.0 * R2)/voltage) - R2;
    
    //--- Only for finding initial R0 value ---
    //BAC = RS_gas / 0.6; /0.26;  // Rs/R0 for air only from datasheet plot (MQ-3 [0.6], MQ-2 [0.26])  
    
    ratio = RS_gas/R0;
    double x = (log10(ratio) - b) / m;
    BAC = pow(10, x);  //BAC in mg/l = ppm
     
  switch (_spec) {
    case 0: result = BAC; break;          // BAC in mg/l or ppm
    case 1: result = BAC/10000; break;    // BAC in %
    case 2: result = voltage; break;      // Voltage
    case 3: result = value; break;        // RAW
    case 4: result = value2; break;       // trigger 0/1
  }
  return result;
}

void MQ2::calibrate() {
  //offset = value;
  offset = analogRead(pin);
}
