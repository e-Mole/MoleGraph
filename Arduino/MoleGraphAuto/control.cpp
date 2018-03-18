#include "Control.h"

// Uncomment to enable printing out nice debug messages.
//#define DEBUG

// Define where debug output will be printed.
#define DEBUG_PRINTER Serial

// Setup debug printing macros.
#ifdef DEBUG
  #define DEBUG_PRINT(...) { DEBUG_PRINTER.print(__VA_ARGS__); }
  #define DEBUG_PRINTLN(...) { DEBUG_PRINTER.println(__VA_ARGS__); }
#else
  #define DEBUG_PRINT(...) {}
  #define DEBUG_PRINTLN(...) {}
#endif 
  
uint8_t newChannel(uint8_t _channel, SensorType _type, uint32_t _period, uint8_t _port, uint8_t _spec) {
  if (_channel >= MAX_CHANNELS || _port >= MAX_PORTS) { 
    DEBUG_PRINTLN("Channel or port out of range!");
    return 0;
  }

  for (uint8_t i = 0; i < MAX_CHANNELS; i++) {
    if (i != _channel && channel[i] != NULL && _port == channel[i]->port && 
          (  (_type != channel[i]->type)
          || (_type == channel[i]->type && _spec == channel[i]->spec))) {
      DEBUG_PRINTLN("Channel conflict!");
      return 0;    
    }
  }

  if (channel[_channel] != NULL) {
    deleteSensor(channel[_channel]->port);
    channelMask &= ~(1 << _channel);
    delete(channel[_channel]);
    channel[_channel] = NULL;
    DEBUG_PRINT("Deleting channel: "); DEBUG_PRINTLN(_channel);
  }  

  if (_type == SENSOR_NONE) return 0;

  newSensor(_type, _period, _port, _spec);

  if (sensor[_port] != NULL) 
  {
    channel[_channel] = new Channel((uint8_t)_type, _port, _spec);

    if (channel[_channel] != NULL) {
      channelMask |= (1 << _channel);
      channelCount++;
    DEBUG_PRINT("Creating channel: "); DEBUG_PRINTLN(_channel);
    } else {
      deleteSensor(_port);
    }
  }  
}

bool newSensor(SensorType _type, uint32_t _period, uint8_t _port, uint8_t _spec) {
  
  if (sensor[_port] == NULL) {
    switch (_type) {
      case SENSOR_DS18B20:  		sensor[_port] = new DS18B20(_period, _port); break;
      case SENSOR_AD:       		sensor[_port] = new AD(_period, _port); break;
      case SENSOR_VL53L0X:  		sensor[_port] = new VL53L0X(_period, _port); break;
      case SENSOR_FORCE:    		sensor[_port] = new Silomer(_period, _port); break;
      case SENSOR_HX711:    		sensor[_port] = new HX711(_period, _port); break;
      case SENSOR_DHT11:    		sensor[_port] = new DHT11(_period, _port); break;
      case SENSOR_MLX90614: 		sensor[_port] = new MLX90614(_period, _port); break;
      case SENSOR_MPX5700DP:		sensor[_port] = new MPX5700DP(_period, _port); break;
      case SENSOR_SRF04:    		sensor[_port] = new SRF04(_period, _port); break;
      case SENSOR_LUX:      		sensor[_port] = new Lux(_period, _port); break;
      case SENSOR_TIMER:    		sensor[_port] = new Timer(_period, _port); break;
      case SENSOR_LED:      		sensor[_port] = new LedBlink(_period, _port); break;
      case SENSOR_MAGNETOMETR:  sensor[_port] = new Magnetometr(_period, _port); break;
      case SENSOR_SOUNDMETER:   sensor[_port] = new Soundmeter(_period, _port); break;
    }
    DEBUG_PRINT("Creating sensor on port: "); DEBUG_PRINTLN(_port);
  }
    
  if (sensor[_port] != NULL) {
    sensorMask |= (1 << _port);
    sensor[_port]->channelCount++;    
  }
}

void deleteSensor(uint8_t _port) {
  if (_port >= MAX_PORTS) return;
  if (sensor[_port] != NULL) {
    DEBUG_PRINT("Count sensor on port "); DEBUG_PRINT(_port);
    DEBUG_PRINT(" is "); DEBUG_PRINTLN(sensor[_port]->channelCount);
    sensor[_port]->channelCount--;
    if (sensor[_port]->channelCount == 0) {
      sensorMask &= ~(1 << _port);
      delete(sensor[_port]);  
      sensor[_port] = NULL;
    DEBUG_PRINT("Deleting sensor on port: "); DEBUG_PRINTLN(_port);
    }      
  }
}



