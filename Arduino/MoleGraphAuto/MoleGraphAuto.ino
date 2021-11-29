#include "core.h"
#include <OneWire.h>
#include <Wire.h>

#define VERSION "ATG_5" //arduino to graph version

uint32_t oldTime = 0;

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
          enableChannels();
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
          stop();   // doplnit uvolneni vsech portu a kanalu
          break;
    case INS_SET_SENSOR:
          setSensor();
          break;
  }
}

void setup() {
  Serial.begin(115200);
  Wire.begin();

  Timer_Init();
  SystemInit();
}

void loop() {
  newTime = getTime();
  if (newTime < oldTime){
    timeOverflowCounter++; //with time step 0.5 us it happen once every ~36 min
    DEBUG_MSG("timeOverflowCounter increased %u", timeOverflowCounter)
  }
  oldTime = newTime;

  if (StartStop) {
    if (running) {
      bool dataReady = update();
      if (scanType == PERIODICAL) {
        //DEBUG_MSG("scanType == PERIODICAL: %d, %d", newTime - time, period)
        if ((firstSample && dataReady) || (uint32_t)(newTime - time) >= period) {
          firstSample = false;
          if ((uint32_t)(newTime - time) >= period){
            time += period;
          }
          scan();
          sendValues();
          while ((uint32_t)(newTime - time) >= period) { // in some cases can "update" and another processing take longer time that is reqiured period it is necessary to increase time properly and inform the App
            time += period;
            missedSamples = true;
          }
        }
      }
      else if (dataReady){
        running = 0;
        scan();
        sendValues();
      }
    }
  }

  if (Serial.available()) {
    serialProcess();
  }

  SystemProcess(newTime);

  if (button) {
    calibrate(button);
  }

}
