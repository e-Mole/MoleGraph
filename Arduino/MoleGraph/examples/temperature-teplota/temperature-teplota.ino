#include "MoleGraph.h"

#define THERMISTOR_PIN 3

MoleGraph moleGraph;

void UpdateGraphChannels(void)
{
    //TEMP (THERMISTOR) - CH1
    int raw1 = analogRead(THERMISTOR_PIN);
    float celsius = -0.1111*raw1 + 102.69; //this is from thermistor calibration
    float test_temp = round(celsius*10);
    moleGraph.SetChannelValue(1, test_temp/10);
}

void setup() 
{    
  moleGraph.Setup();
  moleGraph.SetSendingCallback(&UpdateGraphChannels);
}

void loop() 
{
  moleGraph.CheckInput();
}