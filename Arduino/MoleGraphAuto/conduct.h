#ifndef CONDUCT_h
#define CONDUCT_h

#include <Arduino.h>
#include "ad.h"

class CONDUCT : public AD {
  public:
    CONDUCT(uint32_t, uint8_t);
    virtual float read(uint8_t);    
};


#endif
