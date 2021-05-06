#ifndef HW_h
#define HW_h

#include <Arduino.h>
#include "timer.h"

#define LED_BATT_RED    2
#define LED_BATT_GREEN  3
#define LED_STATUS      13
#define BT_ACTIVE       12

#define AI_BATT         A7
#define AI_BTN          A6

#define PERIOD_SYSTEM   (100 * TICK_PER_MS)

extern bool     StartStop;
extern uint8_t  button;

void SystemInit();
void SystemProcess(uint32_t);

#endif
