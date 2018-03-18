#include "srf04.h"

#define SRF04_WAIT  75

SRF04::SRF04(uint32_t _period, uint8_t _port) : TimerAbstract(_period, _port) {
  trigger = PORTS[port][1];
  pinMode(trigger, OUTPUT);
  delta  = period; 
  active = 0;
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
        value = x * (0.5f * 0.00017315f);
        velocity = (value - value_1) / (period * 0.001f);
        acceleration = (value - 2 * value_1 + value_2) / (period * period * 1e-6f);
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
    case 0: result = value*100; value = NO_DATA; break;            // poloha cm
    case 1: result = velocity; velocity = NO_DATA; break;          // rychlost m/s    //TODO: vraci podivnou hodnotu
    case 2: result = acceleration; acceleration = NO_DATA; break;  // zrychleni m/s2  //TODO: vraci podivnou hodnotu
  }
  return result;
}



