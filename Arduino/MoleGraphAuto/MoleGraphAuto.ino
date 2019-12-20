#include "core.h"
#include <OneWire.h>
#include <Wire.h>

#define VERSION "ATG_5" //arduino to graph version

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
          stop();   // doplnit uvolneni vset portu a kanalu
          break;
    case INS_SET_SENSOR:
          setSensor();
          break;
  }
}

void setup() {
  Serial.begin(115200);
  Wire.begin();

  SysTick_Init();
  Timer_Init();
  SystemInit();
}

void loop() {
  newTime = Millis();

  if (StartStop) {
    if (running) {
      update();
      if (scanType == PERIODICAL) {
        //DEBUG_MSG("scanType == PERIODICAL: %d, %d", newTime - time, period)
        if (newTime - time >= period) {
          time += period;
          scan();
          sendValues();
        }
      } else {
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
