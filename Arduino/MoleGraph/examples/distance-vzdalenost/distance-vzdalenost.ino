#include "MoleGraph.h"

#define TRIG_PIN 6
#define ECHO_PIN 5

int g_distance = 0;

MoleGraph moleGraph;

void UpdateGraphChannels(void)
{
    //DISTANCE (ULTRASOUND) - CH1
    moleGraph.SetChannelValue(1, g_distance); 
}

void setup() 
{    
  moleGraph.Setup();
  moleGraph.SetSendingCallback(&UpdateGraphChannels);
  
  //sensors settings
  // for US sensor on trigpin
  digitalWrite(TRIG_PIN, LOW);  
}

void loop() 
{
  moleGraph.CheckInput();
  if (moleGraph.IsMeasurementInProgress())
    g_distance = get_CM_Distance(TRIG_PIN,ECHO_PIN);
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
