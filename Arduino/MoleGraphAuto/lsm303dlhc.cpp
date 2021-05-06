#include "LSM303DLHC.h"
#include "Adafruit_Sensor/Adafruit_Sensor.h"
#include "Adafruit_LSM303DLHC/Adafruit_LSM303_U.h"

/* Assign a unique ID to this sensor at the same time */
Adafruit_LSM303_Mag_Unified mag = Adafruit_LSM303_Mag_Unified(12345);
Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified(54321);

const float alpha = 0.5;
double fXg = 0;
double fYg = 0;
double fZg = 0;

LSM303DLHC::LSM303DLHC(uint32_t _period, uint8_t _port) : Sensor(_period, _port) {
  //LSM303DLHC setup
  // Accelerometer initialization
  accel.begin();

  // Enable auto-gain + initialization
  mag.enableAutoRange(true);
  mag.begin();
}

bool LSM303DLHC::process() {
  if (Action(period)) {
      /* Get a new sensor event */
      sensors_event_t event1;
      accel.getEvent(&event1);

      sensors_event_t event2;
      mag.getEvent(&event2);

      aX = event1.acceleration.x;
      aY = event1.acceleration.y;
      aZ = event1.acceleration.z;

      mX = event2.magnetic.x;
      mY = event2.magnetic.y;
      mZ = event2.magnetic.z;

	    value = sqrt(aX * aX + aY * aY + aZ * aZ); //total acceleration
	    value2 = sqrt(mX * mX + mY * mY + mZ * mZ); //total magnetic field


      //calculate compass heading
      float Pi = 3.14159;
      // Calculate the angle of the vector y,x
      heading = (atan2(mY, mX) * 180) / Pi;
      // Normalize to 0-360
      if (heading < 0)
      {
        heading = 360 + heading;
      }

      //calculate roll and pitch
      //Low Pass Filter
      fXg = aX * alpha + (fXg * (1.0 - alpha));
      fYg = aY * alpha + (fYg * (1.0 - alpha));
      fZg = aZ * alpha + (fZg * (1.0 - alpha));

      //Roll & Pitch Equations
      roll  = (atan2(-fYg, fZg)*180.0)/M_PI;
      pitch = (atan2(fXg, sqrt(fYg*fYg + fZg*fZg))*180.0)/M_PI;

    time += period;
    return 1;
  }
  return 0;
}

float LSM303DLHC::read(uint8_t _spec) {
  float result = NO_DATA;
  switch (_spec) {
  case 0: result = value; break; //accel total
  case 1: result = value2; break; //mag total
	case 2: result = aX;; break; //accel X
	case 3: result = aY; break; //accel Y
	case 4: result = aZ; break; //accel Z
	case 5: result = mX; break; //mag X
	case 6: result = mY; break; //mag Y
	case 7: result = mZ; break; //mag Z
	case 8: result = heading; break; //heading
	case 9: result = roll; break; //roll
	case 10: result = pitch; break; //pitch
  }
  return result;
}
