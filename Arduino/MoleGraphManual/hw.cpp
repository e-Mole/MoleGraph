#include "hw.h"

void System::init() {
  pinMode(LED_BATT_RED, OUTPUT);  
  pinMode(LED_BATT_GREEN, OUTPUT);  
  pinMode(LED_STATUS, OUTPUT);  
}

uint8_t System::getBtn() {
  uint8_t x = analogRead(AI_BTN) >> 2;  
  uint8_t result = 0;
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

uint8_t System::getBatt() {
  uint8_t x = analogRead(AI_BATT) >> 2;  
  uint8_t result = 0;
    if (x > 111) {  // Battery OK ... 7V
	  result = 2;
    } else if (x > 80) {  // Battery LOW     
	  result = 1;
    } else {  // Battery NO      
	  result = 0;
    }
  return result;  
}

void System::process(uint32_t t, bool running) {
  if ((uint32_t)(t - systemTime) >= PERIOD_SYSTEM) {
    systemTime += PERIOD_SYSTEM;
    digitalWrite(LED_STATUS, running);
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
  }
}

