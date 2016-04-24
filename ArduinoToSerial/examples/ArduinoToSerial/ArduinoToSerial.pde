#include "ArduinoToSerial.h"
#include "HX711.h"

#define THERMISTOR_PIN 3
#define PHOTORESISTOR_PIN 1
#define TRIG_PIN 6
#define ECHO_PIN 5
#define BUTTON_PIN 12

// Attache scales
HX711 scale(A2, A3);

unsigned counter =  0;
ArduinoToSerial arduinoToSerial;

int buttonState = 0;  // variable for reading the pushbutton status

void UpdateGraphChannels(void)
{   
    float value_scale = 0.00981*scale.get_units();
    float test_scale = round(value_scale*100);    
    arduinoToSerial.SetChannelValue(1, test_scale/100);

    // read the state of the pushbutton value:
    arduinoToSerial.SetChannelValue(2, buttonState);
}

void setup() 
{
  //Serial.begin(38400);
  //Serial.println("HX711 Demo");

  // initialize the pushbutton pin as an input:
  pinMode(BUTTON_PIN, INPUT);
    
  arduinoToSerial.Setup(&UpdateGraphChannels);

  scale.set_gain(128);
  scale.set_scale(-58.35);      // TFs val: -58.5, this value is obtained by calibrating the scale with known weights; see the README for details
  scale.tare();                // reset the scale to 0
  
}

void loop() 
{
  arduinoToSerial.InLoop();

  tare_Button();
  
//  float value = scale.get_units();
//  Serial.print("one reading:\t");
//  Serial.println(value, 0);    
//  delay(200);

}

void tare_Button() {
  // read the state of the pushbutton value:
  buttonState = digitalRead(BUTTON_PIN);

  // check if the pushbutton is pressed.
  // if it is, the buttonState is HIGH:
  if (buttonState == HIGH) {
    // tare scale
    scale.tare(); // reset the scale to 0
    //buttonState = 0;
  }  
}

