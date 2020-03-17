#include "ad8232.h"

AD8232::AD8232(uint32_t _period, uint8_t _port) : Sensor(_period, _port) {
  pinLOM  = PORTS[_port][1];   //digital GPIO 01
  pinAO   = PORTS[_port][0];   //analog GPIO 02
  pinLOP  = 8;                 //fixed extender must be in PORT 4 - D8 (black wire)  
  pinSDN  = A3;                //shutdown pin (not used)
  
  pinMode(pinAO, INPUT);
  pinMode(pinLOM, INPUT); // Setup for leads off detection LO -
  pinMode(pinLOP, INPUT); // Setup for leads off detection LO +

  BPM = 0; //heart rate
  counted = false; // peak counted  
}

bool AD8232::process() {
  if (Action(period)) {
	  if((digitalRead(pinLOP) == 1)||(digitalRead(pinLOM) == 1)){
		  value = NAN;
	  }
	  else {
		  // send the value of analog input
		  value = analogRead(pinAO);
		  
		  // process sensor signal
		  if (value >= 410 && counted == false){ // correct threshold value is 550 (~ 2.7V)   
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
		  } else if (value < 410) { // correct threshold value is 410 (~ 2.0V)    
			counted = false;
			value2 = 0; // pulse notdetected (like trigger 0/1)
		  }		  
	  }    
    time += period;
    return 1;
  }
  return 0;
}

float AD8232::read(uint8_t _spec) {
  float result = NO_DATA;
  switch (_spec) {
    case 0: result = value*(5.0f/1024); break; // voltage
    case 1: result = BPM; break; // BPM
    case 2: result = value2; break; // trigger 0/1
	//case 0: result = value*(5.0f/1024); break; // voltage
    //case 0: result = value; break; // RAW
    //case 1: result = value*(5.0f/1024); break; // voltage
  }
  return result;
}
