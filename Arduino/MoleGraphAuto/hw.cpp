#include "hw.h"

bool      StartStop  = 0;
uint32_t  systemTime = 0;
uint8_t   button     = 0;

void SystemInit() {
  pinMode(LED_BATT_RED, OUTPUT);  
  pinMode(LED_BATT_GREEN, OUTPUT);  
  pinMode(LED_STATUS, OUTPUT);  
}

uint8_t lastBtn = 0;

inline uint8_t getBtn() {
  uint8_t x = analogRead(AI_BTN) >> 2;  
  uint8_t result;
  result = 0;
  if (x < 204) result = 4;
  if (x < 153) result = 3;
  if (x < 102) result = 2;
  if (x < 51)  result = 1;
  if (lastBtn != result) {
    lastBtn = result;
  } else {
    result = 0;
  }
  return result;  
}

void SystemProcess(uint32_t t) {
  if ((uint32_t)(t - systemTime) >= PERIOD_SYSTEM) {
    systemTime += PERIOD_SYSTEM;
    if (StartStop) {
      digitalWrite(LED_STATUS, 1);
    } else {
      digitalWrite(LED_STATUS, 0);
      uint8_t x = analogRead(AI_BATT) >> 2;
      if (x > 111) {  // Battery OK ... 7V
        digitalWrite(LED_BATT_GREEN, 1);
        digitalWrite(LED_BATT_RED, 0);
      } else if (x > 80) {  // Battery LOW
        digitalWrite(LED_BATT_GREEN, 0);
        digitalWrite(LED_BATT_RED, 1);        
      } else {  // Battery NO
        digitalWrite(LED_BATT_GREEN, 0);
        digitalWrite(LED_BATT_RED, 0);        
      }
      if (button = getBtn()) {
        digitalWrite(LED_STATUS, 1);
//        Serial.println(button);
      }
    }
  }
}

