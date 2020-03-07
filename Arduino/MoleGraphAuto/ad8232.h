#ifndef AD8232_h
#define AD8232_h

#include <Arduino.h>
#include "systick.h"
#include "Sensor.h"

class AD8232 : public Sensor {
  public:
    AD8232(uint32_t, uint8_t);
    virtual bool process();
    virtual float read(uint8_t);
  private:
    uint8_t   pinAO, pinLOP, pinLOM, pinSDN;
    // START - counting BPM + trigger
	unsigned int value2; 
    unsigned int BPM;
    unsigned long bpmRead[5] = {0,0,0,0,0};
    bool counted = false;      
	// END - counting BPM + trigger
  protected:
    int32_t   offset = 0;	
};

#endif
