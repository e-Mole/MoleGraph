#include "LUXBH1750.h"
#include "BH1750/BH1750.h"

// Create BH1750 object
BH1750 luxMeter;

LUXBH1750::LUXBH1750(uint32_t _period, uint8_t _port) : Sensor(_period, _port) {
	//BH1750 settings
	luxMeter.begin();;
}

bool LUXBH1750::process() {
  if (Action(period)) {
    /* Get a new sensor val */

      value = luxMeter.readLightLevel();; 				//light intensity in lux

    time += period;
    return 1;
  }
  return 0;
}

float LUXBH1750::read(uint8_t _spec) {
  float result = NO_DATA;
  switch (_spec) {
    case 0: result = value; break; 			    //UV intensity
  }
  return result;
}
