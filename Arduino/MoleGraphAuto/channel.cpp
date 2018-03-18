#include "channel.h"

uint8_t channelCount = 0;
uint8_t channelMask = 0;
Channel* channel[MAX_CHANNELS];

Channel::Channel(uint8_t _type, uint8_t _port, uint8_t _spec) {
  type = _type;
  port = _port;
  spec = _spec;
}

float Channel::read() {
  return sensor[port]->read(spec);
}

