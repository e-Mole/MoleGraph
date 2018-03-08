#include "Sensor.h"

volatile uint32_t newTime;

uint8_t PORTS[MAX_PORTS][4] = {
  { 14, 11, 7, PCINT8 },
  { 15, 10, 6, PCINT9 },
  { 16,  9, 5, PCINT10},
  { 17,  8, 4, PCINT11}
};

uint8_t sensorMask = 0;
Sensor* sensor[MAX_PORTS];

Sensor::Sensor(uint8_t _type, uint32_t _period, uint8_t _port) {
  type = _type;
  period = _period;
  port = _port;
  channelCount = 0;
}

Sensor::~Sensor() {
  pinMode(PORTS[port][0], INPUT);  
  pinMode(PORTS[port][1], INPUT);  
  pinMode(PORTS[port][2], INPUT);  
}

void Sensor::start(uint32_t now) {
  time = now - period;
}

void Sensor::stop() {
  time = 0;
}

bool Sensor::process() {
//  if (Millis() - time >= period) {
  if (Action(period)) {
    time += period;
    value = NO_DATA;
    return 1;
  }
  return 0;
}

float Sensor::read(uint8_t _spec) {
  float result = value;
  value = NO_DATA;
  return result;
}

void Sensor::calibrate() {
  value = 0;
}

