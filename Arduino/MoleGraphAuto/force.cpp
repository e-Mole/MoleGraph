#include "force.h"

Force::Force(uint32_t _period, uint8_t _port) : HX711(_period, _port) {
}

float Force::read(uint8_t _spec) {
  offset = (getValue() - offset) / scale;
}

