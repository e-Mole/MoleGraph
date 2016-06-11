#ifndef _ARDUINO_TO_SERIAL_H_
#define _ARDUINO_TO_SERIAL_H_

#if ARDUINO < 100
    #include <WProgram.h>
#else
    #include <Arduino.h>
#endif

class ArduinoToSerial
{ 
public:
  void Setup();
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
  void CheckInput();
};
#endif //_ARDUINO_TO_SERIAL_H_
