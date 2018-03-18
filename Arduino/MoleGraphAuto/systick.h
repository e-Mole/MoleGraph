#ifndef SysTick_h
#define SysTick_h

#include <inttypes.h> 
#include <avr/io.h>
#include <avr/interrupt.h>

extern volatile uint32_t systick;

void SysTick_Init();

inline uint32_t Millis() {
  cli();
  uint32_t result = systick;
  sei();  
  return result;
}

void delay_ms(uint16_t t);

#endif
