#include <Wire.h> 
#include "molegraphmanual.h"
#include "cactus_io_BME280_I2C.h"

#define AD1 PORT_1A
#define trig PORT_3A
#define echo PORT_3D

MoleGraphManual moleGraph;

// Create BME280 object
BME280_I2C bme(0x76); // I2C using address 0x76
// or BME280_I2C bme(0x77); // I2C using address 0x77

void updateGraphChannels(void) {
  int raw = analogRead(AD1);
  moleGraph.setChannelValue(1, raw);
  
  bme.readSensor();
  float temp = bme.getTemperature_C();
  float hum = bme.getHumidity();
  float pre = bme.getPressure_HP();
  moleGraph.setChannelValue(2, temp);
  moleGraph.setChannelValue(3, hum);
  moleGraph.setChannelValue(4, pre);

  moleGraph.setChannelValue(5, fCalculate());
}

float fCalculate(void) {
  long duration;
  float distance;  
  // Clears the trig Pin
  digitalWrite(trig, LOW);
  delay_us(2); //!!! use own delay_us
  // Sets the trig Pin on HIGH state for 10 micro seconds
  digitalWrite(trig, HIGH);
  delay_us(10); //!!! use own delay_us
  digitalWrite(trig, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echo, HIGH);
  // Calculating the distance
  distance = duration * 0.034 / 2;
  // Return the distance
  return distance;
}

void setup() {    
  moleGraph.init();
  moleGraph.setSendingCallback(&updateGraphChannels);
  
  //BME280 on port ID2
  bme.begin();
  
  //SR-04 sonar on port ID3
  pinMode(trig, OUTPUT); // Sets the trig Pin as an Output
  pinMode(echo, INPUT); // Sets the echo Pin as an Input
 
//  if (!bme.begin()) {
//    Serial.println("Could not find a valid BME280 sensor, check wiring!");
//  while (1);
//   } 
  
}

void loop() {
  moleGraph.process();
  bool i = moleGraph.getButton(1);
}
