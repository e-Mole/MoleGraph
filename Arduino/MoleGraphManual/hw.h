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

class System {
  public:
    void     init();
    void     process(uint32_t, bool);
    uint8_t  getBtn();    
	uint8_t  getBatt();   
  private:
    uint8_t  button;
    uint8_t  lastBtn = 0;
	uint8_t  lastBatt = 0;
    uint32_t systemTime = 0;   
};

#endif
