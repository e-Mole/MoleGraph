#include "channel.h"
#include "sensor.h"

Channel::Channel(){
  Reset();
}

void Channel::Reset(){
  sensor = NULL;
  spec = ~0;
  lastData = NO_DATA;
  enabled = false;
}

void Channel::AssignSensor(Sensor* sensor, uint8_t spec)
{
  this->sensor = sensor;
  this->spec = spec;
  this->lastData = NO_DATA; //for sure
}

void Channel::CollectData() {
  if (sensor != NULL){ //otherwise NO_DATA remains
    lastData = sensor->read(spec);
  }
}

