#ifndef Sensor_h
#define Sensor_h

#include <Arduino.h>
#include "systick.h"

#define NO_DATA         -1.0e-8f
#define MAX_PORTS       4

enum SensorType {
  SENSOR_NONE         = 0,    //
  SENSOR_DS18B20      = 1,    // W001 Teplomer DS18B20
  SENSOR_BME280       = 2,    // I002 Barometr + teplomer + vlhkomer BME280
  SENSOR_LSM303DLHC   = 3,    // I003 Akcelerometr + magnetometr LSM303DLHC
  SENSOR_MLX90614     = 5,    // I005 Bezkontaktni teplomer MLX90614
  SENSOR_MPX5700DP    = 6,    // A006 Tlakomer MPX5700DP
  SENSOR_SILOMER      = 7,    // D007 Silomer s HX711
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
  SENSOR_AD           = 101,  // A101 Analogovy vstu 0-5V
  SENSOR_SHARP        = 102,  // A102 Snimac vzdalenosti Sharp GD2D120
  SENSOR_TIMER        = 103,  // D103 Mereni delky pulsu, periody a frekvence digitalniho signalu
  SENSOR_VL53L0X      = 105,  // I105 Dalkomer VL53L0X
  SENSOR_HX711        = 107,  // 
  SENSOR_LED          = 200,
};
  
extern volatile uint32_t newTime;
extern uint8_t PORTS[MAX_PORTS][4];

#define Action(x) (newTime - time >= x) 

class Sensor {
  public:
    Sensor(uint8_t, uint32_t, uint8_t);
    virtual ~Sensor();
    virtual void start(uint32_t);
    virtual void stop();
    virtual bool process();
    virtual float read(uint8_t);
    virtual void calibrate();
    uint8_t channelCount = 0;
    uint8_t type;
    uint32_t  period;
  protected:
    uint32_t  time;
    uint8_t   port;
    float     value = NO_DATA;
};

extern uint8_t sensorMask;
extern Sensor* sensor[MAX_PORTS];

#endif
