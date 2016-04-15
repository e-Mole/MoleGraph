#include "ArduinoToGraph.h"

#define THERMISTOR_PIN 3
#define PHOTORESISTOR_PIN 1
#define TRIG_PIN 6
#define ECHO_PIN 5
#define BUTTON_PIN 12

unsigned counter =  0;
int g_distance = 0;
ArduinoToGraph arduinoToGraph;

void UpdateGraphChannels(void)
{
  /*
   * TODO: replace this demo by a port values reading. 
   * this place is suitable to transfer 10-bit values that you get from A/D converter to the value which you want to display
   * To display a value, please, use channels in the range 1..8.  
   */   
    
    //TEMP (THERMISTOR) - CH1
    int raw1 = analogRead(THERMISTOR_PIN);
    float celsius = -0.1111*raw1 + 102.69;
    float test_temp = round(celsius*10);
    arduinoToGraph.SetChannelValue(1, test_temp/10);
    //arduinoToGraph.SetChannelValue(1, celsius);
    //arduinoToGraph.SetChannelValue(2, test_temp/10);
    //arduinoToGraph.SetChannelValue(8, raw);

    //LIGHT (PHOTORESISITOR) - CH2
    int raw2 = analogRead(PHOTORESISTOR_PIN);
    arduinoToGraph.SetChannelValue(2, map(raw2, 70, 990, 0, 255));
    
    //DISTANCE (ULTRASOUND) - CH3
    arduinoToGraph.SetChannelValue(3, g_distance);
    
}

void setup() 
{    
  arduinoToGraph.SetUpdateCallbackFunction(&UpdateGraphChannels);
  
  /*
   * TODO: replace this demo values by that you prefere
   * 
   */
  arduinoToGraph.Setup(0, 0, 0, 0, 0, 0, 0, 0);

  //sensors settings
   // for US sensor on trigpin
   digitalWrite(TRIG_PIN, LOW );  
}

void loop() 
{
  arduinoToGraph.InLoop();
  if (arduinoToGraph.IsMeasurementInProgress())
    g_distance = get_CM_Distance(TRIG_PIN,ECHO_PIN) ;
}

int get_CM_Distance(int trigPin, int echoPin)
{
  long duration;
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(20);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  duration = duration / 59;
  if ((duration < 2) || (duration > 300)) return false;
  return duration;
}
