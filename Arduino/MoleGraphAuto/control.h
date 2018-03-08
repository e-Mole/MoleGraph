#ifndef Control_h
#define Control_h

#include "hw.h"
#include "sensor.h"
#include "channel.h"

#include "timer.h"
#include "led_blink.h"
#include "ad.h"
#include "ds18b20.h"
#include "srf04.h"
#include "vl53l0x.h"
#include "silomer.h"
#include "dht11.h"
#include "mlx90614.h"
#include "mpx5700dp.h"
#include "lux.h"
#include "hx711.h"

//#define DEBUG

void channelInfo();
uint8_t newChannel(uint8_t _channel, SensorType _type, uint16_t _period, uint8_t _port, uint8_t _spec = 0);

bool newSensor(SensorType _type, uint16_t _period, uint8_t _port, uint8_t _spec = 0);
void deleteSensor(uint8_t _port);

#endif
