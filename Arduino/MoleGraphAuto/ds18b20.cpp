#include "ds18b20.h"

#define DS18B20_WAIT  800

//byte ADDR[] = {0x10, 0x64, 0x55, 0x34, 0x00, 0x00, 0x00, 0xE5};

DS18B20::DS18B20(uint8_t _type, uint32_t _period, uint8_t _port, uint8_t _resolution) : Sensor(_type, _period, _port) {
  pin = PORTS[port][0];
  pinMode(pin, INPUT_PULLUP);
  pinMode(PORTS[port][2], OUTPUT);
  digitalWrite(PORTS[port][2], 1);  // zapnuti vnejsiho pullupu
  delta  = period;
  resolution = _resolution;
  active = 0;
  ds = new OneWire(pin);
  setResolution();
}

void DS18B20::setResolution() {
  uint8_t conf;

  conf = resolution << 5 | 0x1F;
  ds->reset();
  ds->write(0xCC);        // skip address
  ds->write(0x4E);        // write Scratchpad
  ds->write(0);           // TH
  ds->write(0);           // TL
  ds->write(conf);        // Configuration register 
    
}

void DS18B20::startTemp() {
  ds->reset();
  ds->write(0xCC);        // skip address
  ds->write(0x44);        // start conversion 
}

bool DS18B20::get() {
  uint8_t data[9];

  ds->reset();
  ds->write(0xCC);        // skip address
  ds->write(0xBE);        // Read Scratchpad   
  for (uint8_t i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ds->read();
  }

  if (ds->crc8(data, 9) == 0) {      
    int16_t x = data[0] | ((uint16_t)data[1] << 8);
    value =  (float)x * (1.0f/16); 
//    value =  (float)x * (1.0f/2); 
    return 1;
  }
  return 0;
}

bool DS18B20::process() {
//  uint32_t timeStamp = Millis();
  if (Action(delta)) {
//  if (timeStamp - time >= delta) {
    if (active == 0) {
      active = 1;
//      time = timeStamp;   // musime cekat minimalne DS1820_WAIT od ted, ne od minule aktivace (nemusi byt presne dodrzena perioda)
      time += period;   // musime cekat minimalne DS1820_WAIT od ted, ne od minule aktivace (nemusi byt presne dodrzena perioda)
      delta = DS18B20_WAIT >> (3 - resolution);
      startTemp();
    } else {
      active = 0;
      delta  = period;
      return get();
    }
  }
  return 0; 
}




