#include "Sensor.h"

volatile uint32_t newTime;

uint8_t PORTS[MAX_PORTS][4] = {
  { 14, 11, 7, PCINT8 },
  { 15, 10, 6, PCINT9 },
  { 16,  9, 5, PCINT10},
  { 17,  8, 4, PCINT11}
};

Sensor::Sensor(uint32_t _period, uint8_t _port) {
  period = _period;
  port = _port;
}

Sensor::~Sensor() {
  pinMode(PORTS[port][0], INPUT);
  pinMode(PORTS[port][1], INPUT);
  pinMode(PORTS[port][2], INPUT);
}

void Sensor::start(uint32_t now) {
  time = now - period;
}

void Sensor::stop() {
  time = 0;
}

void Sensor::processData(){
  dataReady = process();
}

bool Sensor::process() {
  if (Action(period)) {
    time += period;
    value = NO_DATA;
    return true;
  }
  return false;
}

float Sensor::read(uint8_t _spec) {
  return value;
}

void Sensor::calibrate() {
}

void I2C_WriteByte(uint8_t addr, uint8_t data) {
  Wire.beginTransmission(addr);
  Wire.write(data);
  Wire.endTransmission();
}

void I2C_WriteRegister(uint8_t addr, uint8_t reg, uint8_t data) {
  Wire.beginTransmission(addr);
  Wire.write(reg);
  Wire.write(data);
  Wire.endTransmission();
}

uint8_t I2C_ReadData8(uint8_t addr, uint8_t reg) {
  I2C_WriteByte(addr, reg);
  Wire.requestFrom(addr, (uint8_t)1);
  while (Wire.available() < 1);
  uint8_t b = Wire.read();
  return b;
}

uint16_t I2C_ReadData16BE(uint8_t addr, uint8_t reg) {
  uint8_t d[2];
  I2C_WriteByte(addr, reg);
  Wire.requestFrom(addr, (uint8_t)2);
  while (Wire.available() < 2);
  d[0] = Wire.read();
  d[1] = Wire.read();
  return (uint16_t)d[0] << 8 | d[1];
}

uint16_t I2C_ReadData16LE(uint8_t addr, uint8_t reg) {
  uint8_t d[2];
  I2C_WriteByte(addr, reg);
  Wire.requestFrom(addr, (uint8_t)2);
  while (Wire.available() < 2);
  d[0] = Wire.read();
  d[1] = Wire.read();
  return (uint16_t)d[1] << 8 | d[0];
}
