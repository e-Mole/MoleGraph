#include "MoleGraph.h"

#define PHOTORESISTOR_PIN 1

MoleGraph moleGraph;

void UpdateGraphChannels(void)
{
    //LIGHT (PHOTORESISITOR) - CH1
    int raw2 = analogRead(PHOTORESISTOR_PIN);
    moleGraph.SetChannelValue(1, map(raw2, 70, 990, 0, 255));
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
