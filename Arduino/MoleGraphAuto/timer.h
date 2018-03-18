#ifndef Timer_h
#define Timer_h

#include <inttypes.h> 
#include <avr/io.h>
#include <avr/interrupt.h>

extern volatile uint16_t countRise[4], countFall[4];
extern volatile uint32_t pulsePositive[4], pulseNegative[4];
extern volatile uint32_t periodRise[4], periodFall[4];

void Timer_Init();

uint32_t getTimer();

#include "Sensor.h"

inline void delay_us(uint16_t x) {
  uint16_t t = TCNT1;
  while (TCNT1 - t < x);    
}

class TimerAbstract : public Sensor {
  public:
    TimerAbstract(uint32_t, uint8_t);
    virtual ~TimerAbstract();
    virtual void start(uint32_t);
  protected:
    uint8_t   pin;
};

class Timer : public TimerAbstract {
  public:
    Timer(uint32_t, uint8_t);
    virtual bool process();
    virtual float read(uint8_t);
  protected:
    float     value1, value2, value3, value4, value5;
};

#endif
