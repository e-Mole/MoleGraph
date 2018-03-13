#ifndef MPX5700DP_h
#define MPX5700DP_h

#include <Arduino.h>
#include "systick.h"
#include "AD.h"

class MPX5700DP : public AD {
  public:
    MPX5700DP(uint32_t, uint8_t);
    virtual float read(uint8_t);
    virtual void  calibrate(); 
  private:
    int32_t   offset = 0;
    float     scale; 
};


#endif
