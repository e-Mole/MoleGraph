#include "bme280.h"
#include "cactus_io_BME280_I2C/cactus_io_BME280_I2C.h"

#define BME280_ADDR  0x76
#define SEALEVELPRESSURE_HPA (1013.25)

// Create BME280 object
BME280_I2C bme(BME280_ADDR);

BME280::BME280(uint32_t _period, uint8_t _port) : Sensor(_period, _port) {
	//BME280 settings
	bme.begin();
	bme.setTempCal(-2);// Temp was reading high so subtract 2 degree
}

bool BME280::process() {
  if (Action(period)) {
    /* Get a new sensor val */
    bme.readSensor();

      value = bme.getPressure_HP(); 				//pressure	  Pa
      value2 = bme.getTemperature_C(); 	    //temperature °C
      value3 = bme.getHumidity();						//humidity	  %

    time += period;
    return 1;
  }
  return 0;
}

float BME280::read(uint8_t _spec) {
  float result = NO_DATA;
  switch (_spec) {
    case 0: result = value; break; 			    // pressure
    case 1: result = value2; break;			    // temperature
    case 2: result = value3; break;			    // humidity
    case 3: result = calcAltitude(); break; // altitude m
    case 4: result = calcDewPoint(); break; // dewpoint °C
  }
  return result;
}

float BME280::calcAltitude()
{
    float atmospheric = value / 100.0F;
    return 44330.0 * (1.0 - pow(atmospheric / SEALEVELPRESSURE_HPA, 0.1903));
}

float BME280::calcDewPoint()
{
    float value = 243.04 * (log(value3/100.0) + ((17.625 * value2)/(243.04 + value2)))/(17.625 - log(value3/100.0) - ((17.625 * value2)/(243.04 + value2)));
    return value;
}
