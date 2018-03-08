#include "core.h"

#include <OneWire.h> 
#include <Wire.h> 

#define VERSION "ATG_5" //arduino to graph version 

enum Instructions
{
  INS_NONE = 0, //data
  INS_GET_VERSION = 1,
  INS_SET_TIME = 2,
  INS_SET_FREQUENCY = 3,
  INS_ENABLED_CHANNELS = 4,
  INS_START = 5,
  INS_STOP = 6,
  INS_SET_TYPE = 7,
  INS_GET_SAMPLE = 8,
  INS_PAUSE = 9,
  INS_CONTINUE = 10,
  INS_INITIALIZE = 11, 
  INS_SET_SENSOR = 12 
};
  
void serialProcess() {
  uint8_t instruction = Serial.read();

  switch (instruction) {
    case INS_GET_VERSION: 
          Serial.write(VERSION);
          break;
    case INS_SET_TIME: 
          setPeriod(0);
          break;
    case INS_SET_FREQUENCY: 
          setPeriod(1);
          break;
    case INS_ENABLED_CHANNELS:
          Serial.readBytes(&sendMask, 1);
          break;
    case INS_START:
          start();
          break;
    case INS_STOP:
          stop();
          break;
    case INS_CONTINUE:
          start();
          break;
    case INS_PAUSE:
          stop();
          break;
    case INS_SET_TYPE:
          setScanType();
          break;
    case INS_GET_SAMPLE:
          getSample();
          break;    
    case INS_INITIALIZE: //when desktop application crash, this stay in sending data
          stop();   // doplnit uvolneni vset portu a kanalu
          break;
    case INS_SET_SENSOR:
          setChannel();
          break;    
  }   
} 

void setup() {
  Serial.begin(115200);
  Wire.begin();
  
  SysTick_Init();
  Timer_Init();
  SystemInit();
  
//  newChannel(0, SENSOR_AD, 100, 0);
//  newChannel(1, SENSOR_LED, 500, 1);
//  newSensor(2, SENSOR_SRF04, 100, 2);
//  newSensor(2, SENSOR_TIMER, 100, 2, 0);
//  newSensor(3, SENSOR_SHARP, 100, 3);

//  sendMask = channelMask;    // hack - vyresit pri tvorbe kanalu
}         

void loop() { 
  newTime = Millis();

  if (StartStop) {
    if (running) {
      update();
  
      if (scanType == PERIODICAL) {
        if (newTime - time >= period) {
          time += period;
          scan();
          sendData();
        }
      } else {
        if (dataReady == 0) {
          running = 0;
          scan();
          sendData();
        }
      }
    }
  } 

  if (Serial.available()) {
    serialProcess();
  }

  SystemProcess(newTime);

  if (button) {
    if (sensor[button-1] != NULL) 
      sensor[button-1]->calibrate();      
  }

}
