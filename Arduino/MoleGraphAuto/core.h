#ifndef Core_h
#define Core_h

#include "systick.h"
#include "timer.h"
#include "control.h"

enum ScanType
{
  PERIODICAL = 0,
  ONDEMAND = 1
};

extern bool running;
extern ScanType scanType; 
extern uint32_t time;
extern uint32_t period;
extern uint8_t sendMask;
extern uint8_t dataReady;

void start();
void stop();
void update();
void sendData();
uint8_t setChannel();
void setScanType();
void setPeriod(bool);
void scan();
void getSample();

#endif
