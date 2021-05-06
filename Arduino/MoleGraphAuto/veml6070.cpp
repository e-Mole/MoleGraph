#include "veml6070.h"
#include "Adafruit_VEML6070/Adafruit_VEML6070.h"

// Create VEML6070 object
Adafruit_VEML6070 uv = Adafruit_VEML6070();

VEML6070::VEML6070(uint32_t _period, uint8_t _port) : Sensor(_period, _port) {
	//VEML6070 settings
	uv.begin(VEML6070_1_T);  // pass in the integration time constant
}

bool VEML6070::process() {
  if (Action(period)) {
    /* Get a new sensor val */

      value = uv.readUV(); 				//UV intensity

    time += period;
    return 1;
  }
  return 0;
}

float VEML6070::read(uint8_t _spec) {
  float result = NO_DATA;
  switch (_spec) {
    case 0: result = value; break; 			    //UV intensity
  }
  return result;
}
