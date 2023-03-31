#include "Timer.h"

volatile uint16_t tick = 0;

void timerInit()
{
//  TIMSK0 &= ~(1<<TOIE0);            // zakazani preruseni pri preteceni casovace CTC0 (otestovat)

//  TCCR1A = (0<<WGM11) | (0<<WGM10);	// Normal mode
//  TCCR1B = (0<<WGM12) | (0<<CS12) | (1<<CS11) | (0<<CS10);	// CTC, predelicka 8, 2 MHz, casova zakladna 0,5 us
//  TIMSK1 |= (1<<TOIE1);            // povoleni preruseni pri preteceni

  TCCR1A = (1<<WGM11) | (0<<WGM10);	// FastPWM top ICR1 = 0xFFFF
  TCCR1B = (1<<WGM13) | (1<<WGM12) | (0<<CS12) | (1<<CS11) | (0<<CS10);	// predelicka 8, 2 MHz, casova zakladna 0,5 us
  ICR1 = 0xFFFF;
  TIMSK1 |= (1<<TOIE1);            // povoleni preruseni pri preteceni
}

ISR (TIMER1_OVF_vect) {
  tick++;
}

void Servo::attach(uint8_t port) {
  if (port != 2 && port != 3) return;
  position = 3000;
  if (port == 2) { // servo na portu 2
    pin = 10;
    reg = &OCR1B;
  	TCCR1A |= (1<<COM1B1); // povoleni generovani PWM na vystupu 
  } else {
    pin = 9;
    reg = &OCR1A;
  	TCCR1A |= (1<<COM1A1); // povoleni generovani PWM na vystupu 
  }  
  pinMode(pin, OUTPUT);
  *reg = position;
}

void Servo::detach() {
  if (pin != 9 && pin != 10) return;
  if (pin == 10) { // servo na portu 2
  	TCCR1A &= ~(1<<COM1B1); // zakazani generovani PWM na vystupu 
  } else {
  	TCCR1A &= ~(1<<COM1A1); // zakazani generovani PWM na vystupu 
  }  
  pinMode(pin, INPUT);
}

void Servo::write(uint16_t x) {
  position = x;
  if (position < 200) {
  position = (position - 90) * 100 / 9 + 1500;
  }
  *reg = position * 2;
}

uint16_t Servo::read() {
  return position;
}
