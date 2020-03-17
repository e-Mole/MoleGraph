#include "pulse.h"

PULSE::PULSE(uint32_t _period, uint8_t _port) : Sensor(_period, _port) {
  pin = PORTS[_port][0];
  pinMode(pin, INPUT);
  
  BPM = 0; //heart rate
  // bpmRead[5] = {0,0,0,0,0}; // 5 pulses to calculate BPM
  counted = false; // peak counted
}

bool PULSE::process() {
  if (Action(period)) {
    // read value from analog input  
	value = analogRead(pin);
  
	  // this is for PulseSensor Playground sensor
	  if (value >= 511 && counted == false){ // correct threshold value is 550 (~ 2.7V)   
		counted = true;
		// BPM calculation
		  bpmRead[4] = bpmRead[3];
		  bpmRead[3] = bpmRead[2];
		  bpmRead[2] = bpmRead[1];
		  bpmRead[1] = bpmRead[0];
		  bpmRead[0] = Millis(); //time; //Millis();  
		  if (bpmRead[4] > 0){
			unsigned long timeBPM = bpmRead[0] - bpmRead[4];
			BPM = (60000 / (float)(timeBPM / 5));
		  }
		  value2 = 1; // pulse detected (like trigger 0/1)
	  } else if (value < 511) { // correct threshold value is 550 (~ 2.7V)    
		counted = false;
		value2 = 0; // pulse notdetected (like trigger 0/1)
	  }
	  
    time += period;
    return 1;
  }
  return 0;
}

float PULSE::read(uint8_t _spec) {
  float result = NO_DATA;
  switch (_spec) {
    case 0: result = value; break; // RAW
    case 1: result = BPM; break; // BPM
    case 2: result = value2; break; // trigger 0/1
  }
  return result;
}
