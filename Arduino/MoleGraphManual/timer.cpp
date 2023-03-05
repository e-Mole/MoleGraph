#include "Timer.h"

volatile uint16_t tick = 0;

void timerInit()
{
  TIMSK0 &= ~(1<<TOIE0);            // zakazani preruseni pri preteceni casovace CTC0 (otestovat)

  TCCR1A = (0<<WGM11) | (0<<WGM10);	// Normal mode
  TCCR1B = (0<<WGM12) | (0<<CS12) | (1<<CS11) | (0<<CS10);	// CTC, predelicka 8, 2 MHz, casova zakladna 0,5 us
  TIMSK1 |= (1<<TOIE1);            // povoleni preruseni pri preteceni
}

ISR (TIMER1_OVF_vect) {
  tick++;
}

