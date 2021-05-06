#ifndef Core_h
#define Core_h

#include "systick.h"
#include "hw.h"
#include "sensor.h"
#include "channel.h"
#include "timer.h"
#include "led_blink.h"
#include "ad.h"
#include "ds18b20.h"
#include "srf04.h"
#include "vl53l0x.h"
//#include "force.h"
//#include "dht11.h" //"dht11mg.h"
#include "mlx90614.h"
#include "mpx5700dp.h"
//#include "lux.h" // replaced with universal AD
//#include "hx711.h"
//#include "magnetometr.h" // replaced with universal AD
//#include "soundmeter.h" // replaced with universal AD
#include "silomer.h"
#include "bme280.h"
//#include "mhz16.h"
#include "volt25.h"
#include "amp5.h"
#include "amp30.h"
#include "veml6070.h"
#include "mq3.h"
#include "ph.h"
#include "luxbh1750.h"
#include "lsm303dlhc.h"
#include "pulse.h"
#include "max6675t.h"
#include "ad8232.h"
#include "caliper.h"

// Uncomment to enable printing out nice debug messages.
//#define DEBUG

enum Instructions
{
  INS_NONE = 0,
  INS_GET_VERSION = 1,
  INS_SET_TIME = 2,
  INS_SET_FREQUENCY = 3,
  INS_ENABLED_CHANNELS = 4,
  INS_START = 5,
  INS_STOP = 6,
  INS_SET_TYPE = 7,
  INS_GET_SAMPLE = 8,
  INS_PAUSE = 9,
  INS_CONTINUE = 10,
  INS_INITIALIZE = 11,
  INS_SET_SENSOR = 12,
  INS_DEBUG = 127,
};

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
  SENSOR_DCV25        = 28,   // A028 Cidlo DC napeti 0-25 V (klasicky delic)
  SENSOR_DCA5         = 29,   // A029 Cidlo DC proudu 0-5 A  (s ACS712)
  SENSOR_DCA30        = 30,   // A030 Cidlo DC proudu 0-30 A (s ACS712)
  SENSOR_VEML6070     = 31,   // I031 Cidlo UV zareni (s VEML6070)
  SENSOR_LUXBH1750    = 32,   // I032 Cidlo intenzity svetla (BH1750) - hodnota v luxech
  SENSOR_SHOCKKY31    = 33,   // D033 Cidlo narazu - hodnota 0/1
  SENSOR_MAX6675      = 34,   // D034 Cidlo teploty - MAX6675 + Termoclanek typu K
  SENSOR_CALIPER      = 35,   // D035 Cidlo "delky" - suplera cinska levna alias length sensor
  SENSOR_AD           = 101,  // A101 Analogovy vstup 0-5V
  SENSOR_SHARP        = 102,  // A102 Snimac vzdalenosti Sharp GD2D120
  SENSOR_TIMER        = 103,  // D103 Mereni delky pulsu, periody a frekvence digitalniho signalu
  SENSOR_VL53L0X      = 105,  // I105 Dalkomer VL53L0X
  SENSOR_HX711        = 107,  //
  SENSOR_LED          = 200,
};

enum ScanType
{
  PERIODICAL = 0,
  ONDEMAND = 1
};

extern bool running;
extern ScanType scanType;
extern uint32_t time;
extern bool firstSample;
extern uint32_t period;
extern uint8_t  dataReady;

void enableChannels();
void start();
void stop();
bool update();
void sendCommand(uint8_t command, const char* format_msg, ...);
void sendValues();
void assignPort(uint8_t, uint8_t);
uint8_t setSensor();
void setScanType();
void setPeriod(bool);
void scan();
void getSample();
void calibrate(uint8_t button);

// Setup debug printing macros.
#ifdef DEBUG
  #define DEBUG_MSG(MESSAGE...) { sendCommand(INS_DEBUG,  MESSAGE); }
#else
  #define DEBUG_MSG(MESSAGE, ...) {}
#endif

#endif
