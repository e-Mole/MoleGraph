#include "core.h"

float data[MAX_CHANNELS];

#define PERIOD  1000
uint32_t time, startTime;
uint32_t period = PERIOD;
uint8_t  sendMask = 0;

bool running = 0;
uint8_t dataReady = 0;

ScanType scanType = PERIODICAL; 

 // inicializace vsech pripojenych snimacu
void start() {
  if (StartStop == 0) {
    StartStop = 1;

// hack moleGraph zatim neposila pri vytvareni kanalu Tvz  
    for (uint8_t i = 0; i < MAX_PORTS; i++) {
      if (sensor[i] != NULL) {
        sensor[i]->period = period;
      }
    }   

    time = Millis();
    startTime = time;
    for (uint8_t i = 0; i < MAX_PORTS; i++) {
      if (sensor[i] != NULL) {
        sensor[i]->start(time);
      }
    }   
//    time -= period;
    time -= 5;  // odesilani dat 5ms pred novym samplem, prvni data az po uplynuti periody-5
  }
}

void stop() {
  if (StartStop == 1)
    StartStop = 0;  
}

// update vsech pripojenych snimacu
void update() {
  if (scanType == PERIODICAL) dataReady = sensorMask; // pri periodickem mereni, vzdy merit vse

  uint8_t m = 0x01;
  for (uint8_t i = 0; i < MAX_PORTS; i++) {
    if (sensor[i] != NULL) {
      if (sensor[i]->process()) {
        dataReady &= ~m;        // shodi priznak skenovani daneho kanalu, resp. nahodi priznak platnych dat
      }
    }
    m = m << 1;      
  }  
}

void scan() {
  uint8_t m = 0x01;
  for (uint8_t i = 0; i < MAX_CHANNELS; i++) {
    if (channelMask & m) {
      data[i] = channel[i]->read();
    }
    m = m << 1;      
  }  
}

void getSample() {
  if (StartStop == 1 && running == 0) {
    dataReady = sensorMask;
    running = 1;  
  }
}

#ifdef DEBUG
void sendData() {
  Serial.print(startTime);
  Serial.print("  ");
  Serial.print(newTime-startTime);
  Serial.print("  ");

  uint8_t m = 0x01;
  for (uint8_t i = 0; i < MAX_CHANNELS; i++) {
    if (sendMask & m) {
      Serial.print(data[i]);
      Serial.print("  ");
    }
    m = m << 1;
  }   

  Serial.println();   
}
#else
uint8_t getCheckSum(uint8_t d[], uint8_t l) {
  uint8_t result = 0;
  for (uint8_t i = 0; i < l; i++) {
    uint8_t x = d[i];
    for (uint8_t j = 0; j < 8; j++) {
      if (x & 0x01) result++;
      x = x >> 1;  
    }
  }
  return result;
}

uint8_t WriteHeader(uint8_t x) {
  return x;  
}

void sendData() {
  uint8_t dataToSend[2 + (MAX_CHANNELS + 1) * sizeof(float)];
  uint8_t checkSum = 0;
  uint8_t index = 0;
  
  dataToSend[index] = WriteHeader(0);
  index++;
  
  if (scanType == ONDEMAND) {
    float timeStamp = (newTime - startTime) * 0.001f;
    *(float*)(&dataToSend[index]) = timeStamp;
    index += sizeof(float);
  }
  
  uint8_t m = 0x01;
  for (uint8_t i = 0; i < MAX_CHANNELS; i++) {
    if (channelMask & m) {
      *(float*)(&dataToSend[index]) = data[i];
      index += sizeof(float);
    }
    m = m << 1;
  }   

  dataToSend[index] = getCheckSum(dataToSend, index);  
  Serial.write(dataToSend, index+1);   
}
#endif

struct ChannelSetting {
  uint8_t channel;
  uint8_t port;
  uint8_t sensorType;
  uint8_t spec;
  uint8_t order;  
//  uint32_t period;  
}; 

uint8_t setChannel() {
  ChannelSetting ch;
  Serial.readBytes((uint8_t*)&ch, sizeof(ChannelSetting));
//  newChannel(ch.channel, (SensorType)ch.sensorType, ch.period, ch.port, ch.spec);    
//  newChannel(ch.channel, (SensorType)ch.sensorType, 1000, ch.port, ch.spec);    
  newChannel(ch.channel, (SensorType)ch.sensorType, 1000, ch.port, ch.order);    
}

void setScanType() {
  Serial.readBytes((uint8_t*)&scanType, 1);
  if (scanType == PERIODICAL) {
    running = 1;  
  } else {
    running = 0;
  }
}

void setPeriod(bool f) {
  uint16_t x;
  Serial.readBytes((uint8_t*)&x, 2);
  if (f == 0) {
    period = (uint32_t)x * 1000;    // nastaveni periody vzorkovani v ms [zadano v s]
  } else {
    period = (uint32_t)1000 / x;    // nastaveni periody vzorkovani v ms [zadano v Hz]
  }
// hack moleGraph zatim neposila pri vytvareni kanalu Tvz  
  for (uint8_t i = 0; i < MAX_PORTS; i++) {
    if (sensor[i] != NULL) {
      sensor[i]->period = period;
    }
  }   
   

}
 
