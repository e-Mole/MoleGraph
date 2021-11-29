#include "core.h"
#include "channel.h"
#include "sensor.h"

#define PERIOD  (1000 * TICK_PER_MS)
#define DEBUG_MSG_MAX_SIZE 100
#define MAX_CHANNELS    8

bool missedSamples = false;
uint32_t time, startTime;
uint32_t period = PERIOD;
bool firstSample = false;
bool running = 0;
uint8_t dataReady = 0;
uint16_t timeOverflowCounter = 0;

ScanType scanType = PERIODICAL;

Sensor* sensors[MAX_PORTS];
Channel channels[MAX_CHANNELS];

void clean(){
  DEBUG_MSG("cleaning")
  for (uint8_t index = 0; index < MAX_CHANNELS; ++index){
    channels[index].Reset();
  }

  for (uint8_t index = 0; index < MAX_PORTS; ++index){
    delete sensors[index];
    sensors[index] = NULL;
  }
}

Sensor* createSensor(SensorType _type, uint32_t _period, uint8_t _port) {
  switch (_type) {
    case SENSOR_DS18B20: return new DS18B20(_period, _port);
    case SENSOR_AD: return new AD(_period, _port);
	  case SENSOR_PULS: return new PULSE(_period, _port);
    case SENSOR_VL53L0X: return new VL53L0X(_period, _port);
    case SENSOR_FORCE: return new Silomer(_period, _port);
    //case SENSOR_HX711: return new HX711(_period, _port);
    //case SENSOR_DHT11: return new DHT11(_period, _port);
    //case SENSOR_DHT11: return new DHT11MG(_period, _port);
    case SENSOR_MLX90614: return new MLX90614(_period, _port);
    case SENSOR_MPX5700DP: return new MPX5700DP(_period, _port);
    case SENSOR_SRF04: return new SRF04(_period, _port);
//    case SENSOR_LUX: return new Lux(_period, _port); // replaced with universal AD
    case SENSOR_LUX: return new AD(_period, _port);
    case SENSOR_TIMER: return new Timer(_period, _port);
    case SENSOR_LED: return new LedBlink(_period, _port);
//    case SENSOR_MAGNETOMETR: return new Magnetometr(_period, _port); // replaced with universal AD
    case SENSOR_MAGNETOMETR: return new AD(_period, _port);
//    case SENSOR_SOUNDMETER: return new Soundmeter(_period, _port); // replaced with universal AD
    case SENSOR_SOUNDMETER: return new AD(_period, _port);
    case SENSOR_BME280: return new BME280(_period, _port);
//    case SENSOR_CO2: return new MHZ16(_period, _port);
	  case SENSOR_DCV25: return new VOLT25(_period, _port);
    case SENSOR_DCA5: return new AMP5(_period, _port);
	  case SENSOR_DCA30: return new AMP30(_period, _port);
	  case SENSOR_VEML6070: return new VEML6070(_period, _port);
	  case SENSOR_MQ3: return new MQ3(_period, _port);
	  case SENSOR_PH: return new PH(_period, _port);
	  case SENSOR_LUXBH1750: return new LUXBH1750(_period, _port);
    case SENSOR_LSM303DLHC: return new LSM303DLHC(_period, _port);
	  case SENSOR_MAX6675: return new MAX6675(_period, _port);
	  case SENSOR_AD8232: return new AD8232(_period, _port);
    case SENSOR_CALIPER: return new CALIPER(_period, _port);	  
  }
}

void addChannelSensor(uint8_t _channel, SensorType _type, uint32_t _period, int8_t _port, uint8_t _spec) {
  DEBUG_MSG("adding sensor type: %d to port %d and channel %d", _type, _port, _channel);
  if (_type == SENSOR_NONE){
    DEBUG_MSG("try to add SENSOR_NONE");
    return;
  }

  if (_channel >= MAX_CHANNELS || _port >= MAX_PORTS) {
    DEBUG_MSG("Channel or port out of range! ch: %d, p %d", _channel, _port);
    return ;
  }

  if (sensors[_port] != NULL) {
    DEBUG_MSG("A sensor already assigned to port: %n", _port);
  }
  else{
    sensors[_port] = createSensor(_type, _period, _port);
  }
  channels[_channel].AssignSensor(sensors[_port], _spec);
}

void enableChannels(){
  uint8_t mask;
  Serial.readBytes(&mask, 1);
  for (uint8_t index = 0; index < MAX_CHANNELS; ++index) {
    if ((mask >> index) & 1) {
      DEBUG_MSG("channel %d enabled", index)
      channels[index].Enable();
    }
  }
}

 // measurement start
void start() {
  if (StartStop == 0) {
    StartStop = 1;

// hack moleGraph zatim neposila pri vytvareni kanalu Tvz
    for (uint8_t i = 0; i < MAX_PORTS; i++) {
      if (sensors[i] != NULL) {
        sensors[i]->period = period;
      }
    }

    time = getTime();
    missedSamples = false;
    startTime = time;
    firstSample = true;
    for (uint8_t i = 0; i < MAX_PORTS; i++) {
      if (sensors[i] != NULL) {
        sensors[i]->start(time);
      }
    }
//    time -= period;
    time -= 5 * TICK_PER_MS;  // odesilani dat 5ms pred novym samplem, prvni data az po uplynuti periody-5
  }
}

void stop() {
  if (StartStop == 1)
    StartStop = 0;
}

// update all connected sensor values
bool update() {
  bool ready = true;
  for (uint8_t i = 0; i < MAX_PORTS; i++) {
    if (sensors[i] != NULL) {
      ready &= sensors[i]->processData();
    }
  }
  return ready;
}

void scan() {
  DEBUG_MSG("scanning")
  for (uint8_t index = 0; index < MAX_CHANNELS; index++) {
    if (channels[index].IsEnabled()) {
      channels[index].CollectData();
    }
  }
}

void getSample() {
  DEBUG_MSG("sample ask")
  if (StartStop == 1) {
    running = 1;
  }
}

uint8_t getCheckSum(uint8_t* data, uint8_t size) {
  uint8_t result = 0;
  for (uint8_t i = 0; i < size; i++) {
    uint8_t x = data[i];
    for (uint8_t j = 0; j < 8; j++) {
      if (x & 0x01) result++;
      x = x >> 1;
    }
  }
  return result;
}

void sendCommand(uint8_t command, const char *format_msg, ...) {
  char data[DEBUG_MSG_MAX_SIZE + 3]; // + command + length + checksum
  data[0] = command;

  uint8_t length = 0;
  if (format_msg != NULL){
    va_list args;
    va_start(args, format_msg);
    vsprintf(data + 2,  format_msg, args); //skip command + length
    va_end(args);

    length = strlen(data+2);
  }

  data[1] = (char)length;
  data[length + 2] = (char)getCheckSum((uint8_t*)data, length + 2); //replaced \0 - will be used length
  Serial.write((uint8_t*)data, length + 3);
}

void sendValues() {
  //DEBUG_MSG("sendValues")
  uint8_t data[2 + (MAX_CHANNELS) * sizeof(float)];
  data[0] = missedSamples ? (1 << 7) : 0;
  
  uint8_t index = 1; //skip header
  if (scanType == ONDEMAND) {
    //time stamp is expected in sec. internal timer overflows every ~36 min with time step 0.5 us and uint32 counter.
    float timeStamp = (float)(newTime - startTime) * TIME_BASE + (float)timeOverflowCounter * ((float)0xffffffff * TIME_BASE);
    *(float*)(&data[index]) = timeStamp;
    index += sizeof(float);
  }

  for (uint8_t i = 0; i < MAX_CHANNELS; i++) {
    if (channels[i].IsEnabled()) {
      *(float*)(&data[index]) = channels[i].GetLastValue();
      index += sizeof(float);
    }
  }
  //DEBUG_MSG("checksum = %d, index =  %d", getCheckSum(data, index), index)
  data[index] = getCheckSum(data, index);
  
  bool full = Serial.availableForWrite() <= index + 1; 
  missedSamples |= full;
  if (! full){
    Serial.write(data, index + 1);
    missedSamples = false; //just processed
  }
} 

struct SensorSetting {
  uint8_t channel;
  int8_t port; //may be -1
  uint8_t sensorType;
  uint8_t spec;
  uint8_t order;
};

uint8_t setSensor() {
  SensorSetting ss;
  Serial.readBytes((uint8_t*)&ss, sizeof(SensorSetting));
  addChannelSensor(ss.channel, (SensorType)ss.sensorType, 1000, ss.port, ss.order);
}

void setScanType() {
  DEBUG_MSG("setScanType")
  Serial.readBytes((uint8_t*)&scanType, 1);
  running = scanType == PERIODICAL;

  //It is expected that this message will be send before each measurement start as a first message (befor settings channels and sensors)
  clean();
}

void setPeriod(bool f) {
  uint32_t x = 0;
  Serial.readBytes((uint8_t*)&x, 2);
  if (f == 0) {
    period = 1000 * (uint32_t)TICK_PER_MS * x;    // nastaveni periody vzorkovani v 0.5 us [zadano v s]
    DEBUG_MSG("setPeriod %u [s]", x)
  } else {
    period = 1000 * (uint32_t)TICK_PER_MS / x;    // nastaveni periody vzorkovani v 0.5 us [zadano v Hz]
    DEBUG_MSG("setFreq = %u [Hz]", x)
  }
  DEBUG_MSG("period = %lu [0,5us]", period)
}

void calibrate(uint8_t button){
  DEBUG_MSG("calibratin port %d", button - 1)
  if (sensors[button-1] != NULL) {
      sensors[button-1]->calibrate();
  }
}
