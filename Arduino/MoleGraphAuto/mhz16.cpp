#include "MHZ16.h"

MHZ16::MHZ16(uint32_t _period, uint8_t _port) : Timer(_period, _port) {
}

float MHZ16::read(uint8_t _spec) {
  float result = NO_DATA;
  switch (_spec) {
    case 0: result = value; break;
    case 1: result = value; break; //return same value
  }
  return result;
}
