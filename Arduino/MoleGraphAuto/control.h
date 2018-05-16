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
#include "force.h"
#include "dht11.h"
#include "mlx90614.h"
#include "mpx5700dp.h"
#include "lux.h"
#include "hx711.h"
#include "magnetometr.h"
#include "soundmeter.h"
#include "silomer.h"
#include "bme280.h"
#include "mhz16.h"

enum SensorType {
  SENSOR_NONE         = 0,    //
  SENSOR_DS18B20      = 1,    // W001 Teplomer DS18B20
  SENSOR_BME280       = 2,    // I002 Barometr + teplomer + vlhkomer BME280
  SENSOR_LSM303DLHC   = 3,    // I003 Akcelerometr + magnetometr LSM303DLHC
  SENSOR_MLX90614     = 5,    // I005 Bezkontaktni teplomer MLX90614
  SENSOR_MPX5700DP    = 6,    // A006 Tlakomer MPX5700DP
  SENSOR_FORCE        = 7,    // D007 Silomer s HX711
  SENSOR_AD8232       = 10,   // A010 EKG AD8232
  SENSOR_TCS3200      = 11,   // C011 Cidlo barev TCS3200
  SENSOR_GUVA_S12SD   = 13,   // A013 Cidlo UV zareni GUVA-S12SD
  SENSOR_PH           = 14,   // A014 Cidlo pH
  SENSOR_CON          = 15,   // C015 Cidlo vodivosti
  SENSOR_UI           = 16,   // C016 Voltmetr a ampermetr s HX711
  SENSOR_LUX          = 18,   // A018 Cidlo intenzity osvetleni
  SENSOR_CO2          = 19,   // C019 Cidlo oxidu uhliciteho MH-Z16
  SENSOR_O2           = 20,   // A020 Cidlo kysliku ME2-O2
  SENSOR_SRF04        = 21,   // D021 Cidlo pohybu (vraci: vzdalenost + rychlost + zrychleni)
  SENSOR_PULS         = 22,   // A022 Cidlo srdecniho tepu
  SENSOR_MAGNETOMETR  = 23,   // A023 Cidlo magnetickeho pole
  SENSOR_DHT11        = 24,   // W024 Vlhkost a teplota DHT11
  SENSOR_MQ3          = 25,   // A025 Cidlo alkoholu (plyn) MQ-3
  SENSOR_DSM501       = 26,   // A026 Cidlo prachovych castic DSM501
  SENSOR_SOUNDMETER   = 27,   // A027 Cidlo intenzity zvuku (mikrofon)
  SENSOR_AD           = 101,  // A101 Analogovy vstup 0-5V
  SENSOR_SHARP        = 102,  // A102 Snimac vzdalenosti Sharp GD2D120
  SENSOR_TIMER        = 103,  // D103 Mereni delky pulsu, periody a frekvence digitalniho signalu
  SENSOR_VL53L0X      = 105,  // I105 Dalkomer VL53L0X
  SENSOR_HX711        = 107,  // 
  SENSOR_LED          = 200,
};
  
uint8_t newChannel(uint8_t _channel, SensorType _type, uint32_t _period, uint8_t _port, uint8_t _spec = 0);

bool newSensor(SensorType _type, uint32_t _period, uint8_t _port, uint8_t _spec = 0);
void deleteSensor(uint8_t _port);

#endif
