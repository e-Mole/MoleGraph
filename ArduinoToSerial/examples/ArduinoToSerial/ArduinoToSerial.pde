#include "ArduinoToSerial.h"
ArduinoToSerial arduinoToSerial;

void UpdateGraphChannels(void)
{ 
  //NOTE: All callback functions are called from a timer interupt so they should take as short time as possible 
  //to they don't block another interrupts or a graph drawing. 
  arduinoToSerial.SetChannelValue(1, arduinoToSerial.GetChannelValue(1) == 20 ? 0 : arduinoToSerial.GetChannelValue(1) + 1);
  arduinoToSerial.SetChannelValue(2, arduinoToSerial.GetChannelValue(2) / 2);
}

void MeasurementStartedCallback(void)
{
  digitalWrite(13, HIGH);
}

void MeasurementStoppedCallback(void)
{
  digitalWrite(13, LOW);
}

void MeasurementPausedCallback(void)
{
  digitalWrite(13, LOW);
}

void MeasurementContinuedCallback(void)
{
  digitalWrite(13, HIGH);
}

void setup() 
{
  //It must not be here if you use hardware hannels
  for (int i = 0; i < 8; i++)
    arduinoToSerial.SetChannelValue(i,i);

  //initialize arduinoToSerial module
  arduinoToSerial.Setup();

  //Set callback functions which you will use  
  arduinoToSerial.SetSendingCallback(&UpdateGraphChannels);
  arduinoToSerial.SetMeasurementStartedCallback(&MeasurementStartedCallback);
  arduinoToSerial.SetMeasurementStoppedCallback(&MeasurementStoppedCallback);
  arduinoToSerial.SetMeasurementPausedCallback(&MeasurementPausedCallback);
  arduinoToSerial.SetMeasurementContinuedCallback(&MeasurementContinuedCallback);

  //initialize pin 13 for measurement state initialization 
  pinMode(13, OUTPUT);
}

void loop() 
{
  //
  arduinoToSerial.CheckInput();
}
