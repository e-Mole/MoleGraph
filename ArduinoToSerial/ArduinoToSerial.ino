#include "ArduinoToGraph.h"

unsigned counter =  0;
ArtuinoToGraph arduinoToGraph;

void UpdateGraphChannels(void)
{
  /*
   * TODO: replace this demo by a port values reading. 
   * this place is suitable to transfer 10-bit values that you get from A/D converter to the value which you want to display
   * To display a value, please, use channels in the range 1..8.  
   */   
  arduinoToGraph.SetChannelValue(1, arduinoToGraph.GetChannelValue(1) + 0.5);
  
  if (arduinoToGraph.GetChannelValue(1) > 20)
  {
    arduinoToGraph.SetChannelValue(1, 0);
    arduinoToGraph.SetChannelValue(2, (-100 == arduinoToGraph.GetChannelValue(2)) ? 100 : -100);
  }
  
  arduinoToGraph.SetChannelValue(3, 1.0 / (float)(++counter));
}

void setup() 
{
  arduinoToGraph.SetUpdateCallbackFunction(&UpdateGraphChannels);
  
  /*
   * TODO: replace this demo values by that you prefere
   * 
   */
  arduinoToGraph.Setup(0.5, -100, 2, 40, 50, 60, 70, 80);
}

void loop() 
{
  arduinoToGraph.InLoop();  
}
