<<<<<<< HEAD:ArduinoToSerial/ArduinoToSerial.h
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
=======
#ifndef _MOLE_GRAPH_H_
#define _MOLE_GRAPH_H_

#if ARDUINO < 100
    #include <WProgram.h>
#else
    #include <Arduino.h>
#endif

class MoleGraph
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
#endif //_MOLE_GRAPH_H_
>>>>>>> d586799a2449373615eaf4c0caabc29b22013ab3:Arduino/MoleGraph/MoleGraph.h
