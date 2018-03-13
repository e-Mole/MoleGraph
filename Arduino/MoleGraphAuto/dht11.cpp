#include "dht11.h"

DHT11::DHT11(uint8_t _type, uint32_t _period, uint8_t _port) : Sensor(_type, _period, _port) {
  pin = PORTS[port][0];
  pinMode(pin, INPUT_PULLUP);
  pinMode(PORTS[port][2], OUTPUT);
  digitalWrite(PORTS[port][2], 1);  // zapnuti vnejsiho pullupu
  _maxcycles = microsecondsToClockCycles(1000);  // 1 millisecond timeout for 
}

uint32_t DHT11::expectPulse(bool level) {
  uint32_t count = 0;
  while (digitalRead(pin) == level) {
    if (count++ >= _maxcycles) {
      return 0; // Exceeded timeout, fail.
    }
  }
  return count;
} 

bool DHT11::get() {
  uint32_t cycles[80];
  {
    // Turn off interrupts temporarily because the next sections are timing critical
    // and we don't want any interruptions.
    cli();

    // End the start signal by setting data line high for 40 microseconds.
    digitalWrite(pin, HIGH);
    delay_us(40);

    // Now start reading the data line to get the value from the DHT sensor.
    pinMode(pin, INPUT_PULLUP);
    delay_us(10);  // Delay a bit to let sensor pull data line low.

    // First expect a low signal for ~80 microseconds followed by a high signal
    // for ~80 microseconds again.
    if (expectPulse(LOW) == 0) {
      _lastresult = false;
      return _lastresult;
    }
    if (expectPulse(HIGH) == 0) {
      _lastresult = false;
      return _lastresult;
    }

    // Now read the 40 bits sent by the sensor.  Each bit is sent as a 50
    // microsecond low pulse followed by a variable length high pulse.  If the
    // high pulse is ~28 microseconds then it's a 0 and if it's ~70 microseconds
    // then it's a 1.  We measure the cycle count of the initial 50us low pulse
    // and use that to compare to the cycle count of the high pulse to determine
    // if the bit is a 0 (high state cycle count < low state cycle count), or a
    // 1 (high state cycle count > low state cycle count). Note that for speed all
    // the pulses are read into a array and then examined in a later step.
    for (uint8_t i = 0; i < 80; i += 2) {
      cycles[i]   = expectPulse(LOW);
      cycles[i+1] = expectPulse(HIGH);
    }
  } // Timing critical code is now complete.
  sei();
  
  // Inspect pulses and determine which ones are 0 (high state cycle count < low
  // state cycle count), or 1 (high state cycle count > low state cycle count).
  for (uint8_t i = 0; i < 40; i++) {
    uint32_t lowCycles  = cycles[2*i];
    uint32_t highCycles = cycles[2*i+1];
    if ((lowCycles == 0) || (highCycles == 0)) {
      _lastresult = false;
      return _lastresult;
    }
    data[i/8] <<= 1;
    // Now compare the low and high cycle times to see if the bit is a 0 or 1.
    if (highCycles > lowCycles) {
      // High cycles are greater than 50us low cycle count, must be a 1.
      data[i/8] |= 1;
    }
    // Else high cycles are less than (or equal to, a weird case) the 50us low
    // cycle count so this must be a zero.  Nothing needs to be changed in the
    // stored data.
  }
/*
  Serial.println(data[0], HEX);
  Serial.println(data[1], HEX);
  Serial.println(data[2], HEX);
  Serial.println(data[3], HEX);
  Serial.println(data[4], HEX);
*/
  // Check we read 40 bits and that the checksum matches.
  if (data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF)) {
    _lastresult = true;
    return _lastresult;
  }
  else {
    _lastresult = false;
    return _lastresult;
  } 

}

bool DHT11::process() {
//  uint32_t timeStamp = Millis();
  if (Action(delta)) {
    if (active == 0) {
      digitalWrite(pin, HIGH); 
      active = 1;      
      time += period;
      delta = 250;
    } else if (active == 1) {
      pinMode(pin, OUTPUT);
      digitalWrite(pin, LOW);
      active = 2;      
      delta = 270;
    } else {
      active = 0;
      delta  = period;
      return get();
    }
  }
  return 0; 
}

float DHT11::read(uint8_t _spec) {
  float result = NO_DATA;
//  if (_lastresult) {
    switch (_spec) {
      case 0: result = data[0]; break;  // teplota
      case 1: result = data[2]; break;  // vlhkost
    }
//  }
  return result;
}



