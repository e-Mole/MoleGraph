#include "SysTick.h"

volatile uint32_t systick = 0;

void SysTick_Init()
{
  TCCR2A = (1<<WGM21) | (0<<WGM20);	// CTC, autoreload OCR2A
  TCCR2B = (0<<WGM22) | (1<<CS22) | (0<<CS21) | (0<<CS20);	// CTC, predelicka 64
  OCR2A  = 249;			                // 1 kHz
  TIMSK2 |= (1<<OCIE2A);            // povoleni preruseni pri dosazeni COMPARE MATCH
  TIMSK0 &= ~(1<<TOIE0);            // zakazani preruseni pri preteceni casovace CTC0 (otestovat)
}

ISR (TIMER2_COMPA_vect) {
  systick++;
}

void delay_ms(uint16_t t) {
  uint16_t x = Millis();
  while (Millis() - x < t);
}

/*
inline uint32_t Millis() {
  uint32_t result;
  cli();
  result = systick;
  sei();  
  return result;
}
*/
