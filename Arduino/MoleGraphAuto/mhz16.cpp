#include "mhz16.h"

#define  RECEIVE_TIMEOUT  (100) 

uint8_t MHZ16::cmd_measure[9]       = {0xFF,0x01,0x9C,0x00,0x00,0x00,0x00,0x00,0x63};
uint8_t MHZ16::cmd_calibrateZero[9] = {0xFF,0x01,0x87,0x00,0x00,0x00,0x00,0x00,0x78};

MHZ16::MHZ16(uint32_t _period, uint8_t _port) : Sensor(_period, _port), serial(PORTS[_port][1], PORTS[_port][0], false) {
  serial.begin(9600);
}

bool MHZ16::process() {
  if (Action(period)) {
    measure();
    time += period;
    return 1;
  }
  return 0;
}

void MHZ16::measure()
{
  uint8_t i;
  uint8_t buf[9];
  uint32_t start = millis();
  value = NO_DATA;
  
  serial.flush();

  for (i=0; i<9; i++) {
    serial.write(cmd_measure[i]);
  }

  for (i=0; i<9;) {
    if (serial.available()) {
      buf[i++] = serial.read();
    }
    if (millis() - start > RECEIVE_TIMEOUT) {
      return;
    }
  }

  parse(buf);
}


void MHZ16::calibrate() {
  for (uint8_t i = 0; i < 9; i++) {
    serial.write(cmd_calibrateZero[i]);
  }
}

uint8_t MHZ16::parse(uint8_t *pbuf) {
  uint8_t checksum = 0;

  for (uint8_t i = 0; i < 9; i++) {
    checksum += pbuf[i];
  }

  if (pbuf[0] == 0xFF && pbuf[1] == 0x9C && checksum == 0xFF) {
    value = (uint32_t)pbuf[2] << 24 | (uint32_t)pbuf[3] << 16 | (uint32_t)pbuf[4] << 8 | pbuf[5];
    value2 = value/10000;
  }
}

float MHZ16::read(uint8_t _spec) {
  float result = NO_DATA;
  switch (_spec) {
    case 0: result = value ; value  = NO_DATA; break;  // ppm
    case 1: result = value2; value2 = NO_DATA; break;  // %
    }
  return result;
} 
