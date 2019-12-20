#ifndef Channel_h
#define Channel_h

#include "sensor.h"

class Channel {
    Sensor * sensor;
    uint8_t spec;
    float lastData;
    bool enabled;

  public:
    Channel();
    void Reset();
    void Enable() { enabled = true; }
    bool IsEnabled() { return enabled; }
    void AssignSensor(Sensor* sensor, uint8_t spec);
    void CollectData();
    float GetLastValue() {return lastData;}
};

#endif
