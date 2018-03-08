#include "Control.h"

//#define DB

uint8_t newChannel(uint8_t _channel, SensorType _type, uint16_t _period, uint8_t _port, uint8_t _spec) {
  if (_channel >= MAX_CHANNELS || _port >= MAX_PORTS) 
  // mimo rozsah kanalu nebo portu
    return 0;

  if (channel[_channel] != NULL) {
    deleteSensor(channel[_channel]->port);
    channelMask &= ~(1 << _channel);
    delete(channel[_channel]);
    channel[_channel] = NULL;
#ifdef DB
    Serial.print("Deleting channel: "); Serial.println(_channel);
#endif    
  }  

  if (_type == SENSOR_NONE) return 0;

  if (sensor[_port] != NULL && _type != sensor[_port]->type) {
    return -1; // na danem portu je jiny snimac
  } else {
    newSensor(_type, _period, _port, _spec);
  }

  if (sensor[_port] != NULL) 
  {
    channel[_channel] = new Channel(_port, _spec);

    if (channel[_channel] != NULL) {
      channelMask |= (1 << _channel);
      channelCount++;
#ifdef DB
    Serial.print("Creating channel: "); Serial.println(_channel);
#endif    
    } else {
      deleteSensor(_port);
    }
  }  
}

bool newSensor(SensorType _type, uint16_t _period, uint8_t _port, uint8_t _spec) {
  
  if (_port >= MAX_PORTS) 
  // mimo rozsah kanalu nebo portu
    return 0;

  if (sensor[_port] == NULL) {
    switch (_type) {
      case SENSOR_DS18B20:  sensor[_port] = new DS18B20((uint8_t)_type, _period, _port); break;
      case SENSOR_AD:       sensor[_port] = new AD((uint8_t)_type, _period, _port); break;
      case SENSOR_VL53L0X:  sensor[_port] = new VL53L0X((uint8_t)_type, _period, _port); break;
      case SENSOR_SILOMER:  sensor[_port] = new Silomer((uint8_t)_type, _period, _port); break;
      case SENSOR_HX711:    sensor[_port] = new HX711((uint8_t)_type, _period, _port); break;
      case SENSOR_DHT11:    sensor[_port] = new DHT11((uint8_t)_type, _period, _port); break;
      case SENSOR_MLX90614: sensor[_port] = new MLX90614((uint8_t)_type, _period, _port); break;
      case SENSOR_MPX5700DP:sensor[_port] = new MPX5700DP((uint8_t)_type, _period, _port); break;
      case SENSOR_SRF04:    sensor[_port] = new SRF04((uint8_t)_type, _period, _port); break;
      case SENSOR_LUX:      sensor[_port] = new Lux((uint8_t)_type, _period, _port); break;   
      case SENSOR_TIMER:    sensor[_port] = new Timer((uint8_t)_type, _period, _port); break;   
      case SENSOR_LED:      sensor[_port] = new LedBlink((uint8_t)_type, _period, _port); break;
    }
#ifdef DB
    Serial.print("Creating sensor on port: "); Serial.println(_port);
#endif    
  }
    
  if (sensor[_port] != NULL) {
    sensorMask |= (1 << _port);
    sensor[_port]->channelCount++;    
  }
}

void deleteSensor(uint8_t _port) {
  if (_port >= MAX_PORTS) return;
  if (sensor[_port] != NULL) {
#ifdef DB
    Serial.print("Count sensor on port "); Serial.print(_port);
    Serial.print(" is "); Serial.println(sensor[_port]->channelCount);
#endif          
    sensor[_port]->channelCount--;
    if (sensor[_port]->channelCount == 0) {
      sensorMask &= ~(1 << _port);
      delete(sensor[_port]);  
      sensor[_port] = NULL;
#ifdef DB
    Serial.print("Deleting sensor on port: "); Serial.println(_port);
#endif          
    }      
  }
}

void channelInfo() {
  for (uint8_t i = 0; i < MAX_CHANNELS; i++) {
    Serial.print(i);
    Serial.print(":");
    Serial.println((uint16_t)channel[i]);
  }
}



