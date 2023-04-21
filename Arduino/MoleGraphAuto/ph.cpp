#include "ph.h"

PH::PH(uint32_t _period, uint8_t _port) : Sensor(_period, _port) {
  pin       = PORTS[_port][0];
  pin_digi  = PORTS[_port][1];
  pinMode(pin, INPUT);
  pinMode(pin_digi, INPUT);
}

bool PH::process() {
  if (Action(period)) {
    value = analogRead(pin);
    value2 = digitalRead(pin_digi);
    time += period;
    return 1;
  }
  return 0;
}

float PH::read(uint8_t _spec) {
  float result = NO_DATA;
  float pH_probe;
  float voltage;

  // Calculating pH from analog output Po
    voltage = value*(5.0f/1024);
	//-------- Calibration - NOT IMPLEMENTED NOW! --------
    //    *** require HW calibration woth trimpot ***
    //	    Set voltage to 2.5 V if BNC is shorted
	//----------------------------------------------------
	// PH_step = (voltage@PH7 - voltage@PH4) / (PH7 - PH4)
    // PH_probe = PH7 - ((voltage@PH7 - voltage@probe) / PH_step)
	// voltage@PH7 = 2.5
    pH_probe = 7 + ((2.5 - voltage) / 0.357);

  switch (_spec) {
    case 0: result = pH_probe; break; // pH
    case 1: result = voltage; break;   // Voltage
    case 2: result = value; break;       // RAW
    case 3: result = value2; break;     // trigger 0/1
  }
  return result;
}
