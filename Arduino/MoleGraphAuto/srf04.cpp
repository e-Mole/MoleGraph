#include "srf04.h"
#include "core.h"

#define SRF04_WAIT  (75 * TICK_PER_MS)

SRF04::SRF04(uint32_t _period, uint8_t _port) : TimerAbstract(_period, _port) {
  trigger = PORTS[port][0];
  pinMode(trigger, OUTPUT);
}

bool SRF04::process() {
  if (Action(delta)) {
    if (active == 0) {
      digitalWrite(trigger, LOW);
      delay_us(2);
      digitalWrite(trigger, HIGH);
      delay_us(10);
      digitalWrite(trigger, LOW); 
      active = 1;
      time += period; 
      delta = SRF04_WAIT;
  } else {
      uint32_t x;
      cli();
      x = pulsePositive[port];
      pulsePositive[port] = 0;     
      sei();
      if (x != 0) {
        value_2 = value_1;
        value_1 = value;
        value = x * (TIME_BASE * 173.15f);
        position = value;
        if (value_1 != NO_DATA) {
          float dT = period * TIME_BASE;
          velocity = (value - value_1) / dT;
          if (value_2 != NO_DATA) {
            acceleration = (value - 2 * value_1 + value_2) / (dT * dT);
          }
        }
      }
      active = 0; 
      delta  = period; 
      return 1;
    } 
  }
  return 0;
}

float SRF04::read(uint8_t _spec) {
  float result = NO_DATA;
  switch (_spec) {
    case 0: result = position; position = NO_DATA; break;  // poloha
    case 1: result = velocity; velocity = NO_DATA; break;  // rychlost
    case 2: result = acceleration; acceleration = NO_DATA; break;  // zrychleni
  }
  return result;
}

void SRF04::start(uint32_t now) {
  Sensor::start(now);
  delta  = period; 
  active = 0;
}



