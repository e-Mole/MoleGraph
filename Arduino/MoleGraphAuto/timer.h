#ifndef Timer_h
#define Timer_h

#include <inttypes.h> 
#include <avr/io.h>
#include <avr/interrupt.h>

#define TIME_BASE 0.5e-6f
#define TICK_PER_US 2
#define TICK_PER_MS 2000UL

extern volatile uint16_t countRise[4], countFall[4];
extern volatile uint32_t pulsePositive[4], pulseNegative[4];
extern volatile uint32_t periodRise[4], periodFall[4];

extern volatile uint16_t tick;

void Timer_Init();

inline static uint32_t getCTC() {
  uint32_t result;
  asm volatile(
    "lds %C0, tick" "\n\t"
    "lds %D0, tick+1" "\n\t"
    "lds %A0, %1" "\n\t"
    "lds %B0, (%1)+1" "\n\t"
    "sbis %2, 0" "\n\t"
    "rjmp skip" "\n\t"
    "cpi  %B0, 255" "\n\t"
    "brcc skip" "\n\t"
    "adiw %C0, 1" "\n\t"
    "skip:" 
    : "=w" (result)
    : "M" (_SFR_MEM_ADDR(TCNT1)), "I" _SFR_IO_ADDR(TIFR1)
  );
  return result;
}

inline static uint32_t getTime() {
  uint32_t result;
  uint8_t  oldSREG = SREG;
  
  cli();
  result = getCTC();
  SREG = oldSREG;
  
  return result;
} 

inline uint32_t Millis() {
  uint32_t result = getTime();  
  return result / TICK_PER_MS;
}
 
#include "sensor.h"

inline void delay_us(uint16_t x) {
  uint16_t t = TCNT1;
  while ((uint16_t)(TCNT1 - t) < x * TICK_PER_US);    
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
    virtual void calibrate();
  protected:
    float     value1, value2, value3, value4, value5;
    uint32_t  counter;
};

#endif
