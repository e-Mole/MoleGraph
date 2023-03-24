#include "Timer.h"

#define PC_PINS   PINB
#define PC_IEx    PCIE0
#define PC_MSK    PCMSK0
#define PC_INT    PCINT0_vect

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
  TIMSK0 &= ~(1<<TOIE0);            // zakazani preruseni pri preteceni casovace CTC0 (otestovat)

  TCCR1A = (0<<WGM11) | (0<<WGM10);	// Normal mode
  TCCR1B = (0<<WGM12) | (0<<CS12) | (1<<CS11) | (0<<CS10);	// CTC, predelicka 8, 2 MHz, casova zakladna 0,5 us
  TIMSK1 |= (1<<TOIE1);            // povoleni preruseni pri preteceni

  PC_MSK  = 0;                     // Zakazani preruseni od zmeny pinu PB0..PB3
  PCICR  |= (1<<PC_IEx);           // Povoleni preruseni od zmeny na portu B

  lastState = PC_PINS & maskPC;
}

ISR (TIMER1_OVF_vect) {
  tick++;
}

ISR (PC_INT) {
  uint32_t temp =  getCTC();
  uint8_t  a = PC_PINS & maskPC;
  uint8_t  b = a ^ lastState;

  if (b != 0) {
    uint8_t index = 4;
    uint8_t c = a & b;
    while (b != 0) {
      index--;
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

TimerAbstract::TimerAbstract(uint32_t _period, uint8_t _port) : Sensor(_period, _port) {
  pin = PORTS[port][1];
  pinMode(pin, INPUT_PULLUP);
  maskPC |= (1 << (pin & 0x07));
  PC_MSK |= (1 << PORTS[port][3]);  
}

TimerAbstract::~TimerAbstract() {
  PC_MSK &= ~(1 << PORTS[port][3]);      
}

void TimerAbstract::start(uint32_t now) {
  uint32_t temp =  TCNT1 | (uint32_t)tick << 16;
  timeRise[port] = temp;
  timeFall[port] = temp;
  countFall[port] = 0;
  countRise[port] = 0;  
  //counter = 0;
  Sensor::start(now);
}

Timer::Timer(uint32_t _period, uint8_t _port) : TimerAbstract(_period, _port) {
}

bool Timer::process() {
  if (Action(period)) {
    uint32_t x0, x1, x2, x3, x4, x5;
    cli();
    x0 = pulsePositive[port]; //délka pulzu na úrovni 1 (s)
    pulsePositive[port] = 0;  
    x1 = pulseNegative[port]; //délka pulzu na úrovni 0 (s)   
    pulseNegative[port] = 0;     
    x2 = periodRise[port]; //perioda měřená na vzestupné hraně (s)    
    periodRise[port] = 0;     
    x3 = periodFall[port]; //perioda měřená na sestupné hraně (s)
    periodFall[port] = 0;     
    counter += countRise[port]; // pulse counter
    x4 = countRise[port]; //frekvence na vzestupné hraně (Hz)
    countRise[port] = 0;     
    x5 = countFall[port]; //frekvence na sestupné hraně (Hz)
    countFall[port] = 0;     
    sei();
    if (x0 != 0) value  = x0 * TIME_BASE;
    if (x1 != 0) value1 = x1 * TIME_BASE;
    if (x2 != 0) value2 = x2 * TIME_BASE;
    if (x3 != 0) value3 = x3 * TIME_BASE;
    value4 = x4 * (1 / TIME_BASE / period);
    value5 = x5 * (1 / TIME_BASE / period);
    time += period;
    return 1;
  }
  return 0;
}

float Timer::read(uint8_t _spec) {
  float result = value;
  switch (_spec) {
    case 0: result = value; break;
    case 1: result = value1; break;
    case 2: result = value2; break;
    case 3: result = value3; break;
    case 4: result = value4; break;
    case 5: result = value5; break;
    case 6: result = counter; break; //counter
  }
  return result;
}

void Timer::calibrate() {
  counter = 0; //reset counter
}
