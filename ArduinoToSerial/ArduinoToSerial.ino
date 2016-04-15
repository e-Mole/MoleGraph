#include "ArduinoToGraph.h"

unsigned counter =  0;
ArduinoToGraph arduinoToGraph;

void UpdateGraphChannels(void)
{
  /*
   * TODO: replace this demo by a port values reading. 
   * this place is suitable to transfer 10-bit values that you get from A/D converter to the value which you want to display
   * To display a value, please, use channels in the range 1..8.  
   * 
   * WARNING:
   * here can be placed value assignmets only. This place is unsuitable for code taking a time. 
   * It will cause a sample missing in a measurement.    
   */   
  static bool firstTurn = true;
  if(firstTurn)
  {
    firstTurn = false;

    arduinoToGraph.SetChannelValue(1, 0.5);
    arduinoToGraph.SetChannelValue(2, -10);
    arduinoToGraph.SetChannelValue(3, 2);
    arduinoToGraph.SetChannelValue(4, 4);
    arduinoToGraph.SetChannelValue(5, 5);
    arduinoToGraph.SetChannelValue(6, 6);
    arduinoToGraph.SetChannelValue(7, 7);
    arduinoToGraph.SetChannelValue(8, 8);
    
    return;
  }
 
  arduinoToGraph.SetChannelValue(1, arduinoToGraph.GetChannelValue(1) + 0.5);
  
  if (arduinoToGraph.GetChannelValue(1) > 20)
  {
    arduinoToGraph.SetChannelValue(1, 0);
    arduinoToGraph.SetChannelValue(2, (-10 == arduinoToGraph.GetChannelValue(2)) ? 10 : -10);
  }
  
  arduinoToGraph.SetChannelValue(3, 1.0 / (float)(++counter));
}

void setup() 
{ 
  arduinoToGraph.Setup(&UpdateGraphChannels);
}

void loop() 
{
  arduinoToGraph.InLoop();  
}
