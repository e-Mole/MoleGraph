#ifndef MAX6675_h
#define MAX6675_h

#include <Arduino.h>
#include "systick.h"
#include "Sensor.h"

class MAX6675 : public Sensor {
  public:
    MAX6675(uint32_t, uint8_t);
    virtual bool  process();
    virtual float read(uint8_t);
    virtual void  calibrate();
  private:
    uint8_t   pinSO, pinCS, pinSCK;
	uint8_t spiread(void);
  protected:
    double   getValue();
    int32_t   offset = 0;
};


#endif
