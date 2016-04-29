#ifndef _ARDUINO_TO_SERIAL_H_
#define _ARDUINO_TO_SERIAL_H_

#if ARDUINO < 100
    #include <WProgram.h>
#else
    #include <Arduino.h>
#endif

class ArduinoToSerial
{
  enum Instructions
  {
    INS_NONE = 0, //data
    INS_GET_VERSION = 1,
    INS_SET_TIME = 2,
    INS_SET_FREQUENCY = 3,
    INS_ENABLED_CHANNELS = 4,
    INS_START = 5,
    INS_STOP = 6,
    INS_SET_TYPE = 7,
    INS_GET_SAMLPE = 8,
    INS_PAUSE = 9,
    INS_CONTINUE = 10
  };

  
public:
  void Setup();
  void InLoop();
  void SetSendingCallback(void (*function)(void));
  void SetMeasurementStartedCallback(void (*function)(void));
  void SetMeasurementStoppedCallback(void (*function)(void));
  void SetMeasurementPausedCallback(void (*function)(void));
  void SetMeasurementContinuedCallback(void (*function)(void));
  
  bool SetChannelValue(int channel, float value);
  float GetChannelValue(int channel);
  void SampleRequest();
  void StartMeasurement();
  void StopMeasurement();
  bool IsMeasurementInProgress();
};
#endif //_ARDUINO_TO_SERIAL_H_
