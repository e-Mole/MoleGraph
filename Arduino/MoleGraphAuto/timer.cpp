#include "Timer.h"

#define PC_PINS   PINC
#define PC_IEx    PCIE1
#define PC_MSK    PCMSK1
#define PC_INT    PCINT1_vect

const uint8_t PC[4] = {(1 << PORTS[0][3]), (1 << PORTS[1][3]), (1 << PORTS[2][3]), (1 << PORTS[3][3])};

volatile uint16_t tick = 0;

volatile uint32_t timeRise[4], timeFall[4];
volatile uint16_t countRise[4], countFall[4];
volatile uint32_t pulsePositive[4], pulseNegative[4];
volatile uint32_t periodRise[4], periodFall[4];
volatile uint8_t  maskPC = 0;
volatile uint8_t  lastState;

void Timer_Init()
{
  TCCR1A = (0<<WGM21) | (0<<WGM20);	// Normal mode
  TCCR1B = (0<<WGM22) | (0<<CS12) | (1<<CS11) | (0<<CS10);	// CTC, predelicka 8, 2 MHz
  TIMSK1 |= (1<<TOIE1);            // povoleni preruseni pri preteceni

  PC_MSK  = 0;                     // Zakazani preruseni od zmeny pinu PC0..PC3
  PCICR  |= (1<<PC_IEx);           // Povoleni preruseni od zmeny na portu C

  lastState = PC_PINS & maskPC;
}

ISR (TIMER1_OVF_vect) {
  tick++;
}

inline uint32_t getTimer() {
  cli();
  uint32_t result =  TCNT1 | (uint32_t)tick << 16;
  sei();
  return result;
}

ISR (PC_INT) {
  uint32_t temp =  TCNT1 | (uint32_t)tick << 16;
  uint8_t  a = PC_PINS & maskPC;
  uint8_t  b = a ^ lastState;

  if (b != 0) {
    uint8_t index = -1;
    uint8_t c = a & b;
    while (b != 0) {
      index++;
      b = b >> 1;
    }
    if (c == 0) { // sestupna hrana
      countFall[index]++;
      pulsePositive[index] = temp - timeRise[index];
      periodFall[index] = temp - timeFall[index];
      timeFall[index] = temp;
    } else {
      countRise[index]++;
      pulseNegative[index] = temp - timeFall[index];
      periodRise[index] = temp - timeRise[index];
      timeRise[index] = temp;    
    }
  }
  lastState = a;
}

TimerAbstract::TimerAbstract(uint8_t _type, uint32_t _period, uint8_t _port) : Sensor(_type, _period, _port) {
  pin = PORTS[port][0];
  pinMode(pin, INPUT_PULLUP);
  maskPC |= (1 << (pin % 0x07));
  PC_MSK |= (1 << PORTS[port][3]);  
}

TimerAbstract::~TimerAbstract() {
  PC_MSK &= ~(1 << PORTS[port][3]);      
}

void TimerAbstract::start(uint32_t now) {
  uint32_t temp =  TCNT1 | (uint32_t)tick << 16;
  timeRise[port] = temp;
  timeFall[port] = temp;
  Sensor::start(now);
}

Timer::Timer(uint8_t _type, uint32_t _period, uint8_t _port) : TimerAbstract(_type, _period, _port) {
}

bool Timer::process() {
  if (Action(period)) {
    uint32_t x0, x1, x2, x3, x4, x5;
    cli();
    x0 = pulsePositive[port];
    pulsePositive[port] = 0;     
    x1 = pulseNegative[port];
    pulseNegative[port] = 0;     
    x2 = periodRise[port];
    periodRise[port] = 0;     
    x3 = periodFall[port];
    periodFall[port] = 0;     
    x4 = countRise[port];
    countRise[port] = 0;     
    x5 = countFall[port];
    countFall[port] = 0;     
    sei();
    if (x0 != 0) value  = x0 * (0.5e-6);
    if (x1 != 0) value1 = x1 * (0.5e-6);
    if (x2 != 0) value2 = x2 * (0.5e-6);
    if (x3 != 0) value3 = x3 * (0.5e-6);
    value4 = x4 * (1000.0f / period);
    value5 = x5 * (1000.0f / period);
    time += period;
    return 1;
  }
  return 0;
}

float Timer::read(uint8_t _spec) {
  float result = value;
  switch (_spec) {
    case 0: result = value;  value  = NO_DATA; break;
    case 1: result = value1; value1 = NO_DATA; break;
    case 2: result = value2; value2 = NO_DATA; break;
    case 3: result = value3; value3 = NO_DATA; break;
    case 4: result = value4; value4 = NO_DATA; break;
    case 5: result = value5; value5 = NO_DATA; break;
  }
  return result;
}


