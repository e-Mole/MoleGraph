#ifndef Channel_h
#define Channel_h

#include "sensor.h"

#define MAX_CHANNELS    8

class Channel {
  public:
    Channel(uint8_t _type, uint8_t _port, uint8_t _spec = 0);
    float   read();
    uint8_t type;
    uint8_t port = 255;
    uint8_t spec;  
  protected:
};

extern uint8_t  channelCount;
extern uint8_t  channelMask;
extern Channel* channel[MAX_CHANNELS];

#endif
