#include "molegraphmanual.h"

#define DEBUG_MSG_MAX_SIZE 100

System runtem;
//MoleGraph moleGraph;

const uint8_t PINS_DIGITAL[] = {PORT_1D, PORT_2D, PORT_3D, PORT_4D};
const uint8_t PINS_ANALOG[]  = {PORT_1A, PORT_2A, PORT_3A, PORT_4A};
const uint8_t PINS_PULLUP[]  = {PORT_1U, PORT_2U, PORT_3U, PORT_4U};
 
void MoleGraph::init() {
  Serial.begin(115200);
  timerInit();
  runtem.init();
}

bool MoleGraph::getButton(uint8_t index) {
  bool result = 0;
  if (index > 0 && index <= 4) {
    result = runtem.getBtn() & (1 << (index - 1)); 
  } 
  return result;
}

uint8_t MoleGraph::getBattery() {
  uint8_t result = runtem.getBatt();  
  return result;
}

void MoleGraph::setPullup(uint8_t index, bool pull) {
  if (index > 0 && index <= 4) {
    if (pull) {
      pinMode(PINS_PULLUP[index-1], OUTPUT);
      digitalWrite(PINS_PULLUP[index-1], 1);
    } else {
      pinMode(PINS_PULLUP[index-1], INPUT);
    } 
  }   
}

// measurement start/continue
void MoleGraph::startMeasurement(bool restart) {
  DEBUG_MSG("starting\n");
  if (measurementInProgress == 0) {
    fullWriteBufferDetected = 0;
    measurementInProgress = 1;
    sampleRequestFlag = 0;
    time = getTime();
    if (restart == 0) {
      if (measurementStartedCallback != NULL) measurementStartedCallback();  
      startTime = time;
    } else {
      if (measurementContinuedCallback != NULL) measurementContinuedCallback();  
    }
    time -= period;
  }
}

// measurement stop/pause
void MoleGraph::stopMeasurement(bool pause) {
  DEBUG_MSG("stoping\n");
  if (measurementInProgress == 1) {
    measurementInProgress = 0;
    if (pause == 0) {
      if (measurementStoppedCallback != NULL) measurementStoppedCallback();
    } else {
      if (measurementPausedCallback != NULL) measurementPausedCallback();
    }
  }
}

void MoleGraph::process() {
  if (Serial.available()) {   
    uint8_t instruction = Serial.read();
    switch (instruction) {
      case INS_GET_VERSION:       Serial.write(VERSION); break;
      case INS_SET_TIME:          setPeriod(0); break;
      case INS_SET_FREQUENCY:     setPeriod(1); break;
      case INS_ENABLED_CHANNELS:  enableChannels(); break;
      case INS_START:             startMeasurement(0); break;
      case INS_STOP:              stopMeasurement(0); break;
      case INS_CONTINUE:          startMeasurement(1); break;
      case INS_PAUSE:             stopMeasurement(1); break;
      case INS_SET_TYPE:          setScanType(); break;
      case INS_GET_SAMPLE:        sampleRequest(); break;
      case INS_INITIALIZE:        stopMeasurement(); break;
      defaut:                     break;
    }
  }
    
  actualTime = getTime();
  update();
  runtem.process(actualTime, measurementInProgress);
}

bool MoleGraph::setChannelValue(uint8_t channel, float value) {
  if (channel >= 1 && channel <= MAX_CHANNELS) {
    channels[channel - 1] = value;
  }
}

float MoleGraph::getChannelValue(uint8_t channel) {
  float value = 0;
  if (channel >= 1 && channel <= MAX_CHANNELS) {
    channels[channel - 1] = value;
  }  
  return value;
}
  
bool  MoleGraph::isMeasurementInProgress() {
  return measurementInProgress;
}
 
void MoleGraph::update() {
  if (measurementInProgress) {
    if (scanType == PERIODICAL) {
      if (actualTime - time >= period) {
        DEBUG_MSG("periodic scanning\n");     
        sampleRequestFlag = 1;
        time += period;
      }
    } 
    if (sampleRequestFlag == 1) {
      DEBUG_MSG("scanning\n");             
      sampleRequestFlag = 0;
      updateFunction();
      sendData();      
    }
  }
}

void MoleGraph::sampleRequest() {
  DEBUG_MSG("sample ask\n")
  if (measurementInProgress == 1) {
    sampleRequestFlag = 1;
  }
}

uint8_t MoleGraph::getCheckSum(uint8_t* data, uint8_t size) {
  uint8_t result = 0;
  for (uint8_t i = 0; i < size; i++) {
    uint8_t x = data[i];
    for (uint8_t j = 0; j < 8; j++) {
      if (x & 0x01) result++;
      x = x >> 1;
    }
  }
  return result;
}

uint8_t MoleGraph::writeHeader(uint8_t x) {
  return x;
}

void MoleGraph::sendCommand(uint8_t command, const char *format_msg, ...) {
  char data[DEBUG_MSG_MAX_SIZE + 3]; // + command + length + checksum
  data[0] = command;

  uint8_t length = 0;
  if (format_msg != NULL){
    va_list args;
    va_start(args, format_msg);
    vsprintf(data + 2,  format_msg, args); //skip command + length
    va_end(args);
    length = strlen(data+2);
  }

  data[1] = (char)length;
  data[length + 2] = (char)getCheckSum((uint8_t*)data, length + 2); //replaced \0 - will be used length
  Serial.write((uint8_t*)data, length + 3);
}

void MoleGraph::sendData() {
  DEBUG_MSG("sendValues\n")
  uint8_t data[2 + (MAX_CHANNELS) * sizeof(float)];
  uint8_t index = 1; //skip header

  data[0] = writeHeader(0);

  if (scanType == ONDEMAND) {
    float timeStamp = (actualTime - startTime) * TIME_BASE;
    *(float*)(&data[index]) = timeStamp;
    index += sizeof(float);
  }

  for (uint8_t i = 0; i < MAX_CHANNELS; i++) {
    if (0 != ((enabledChannels >> i) & 0x01)) {
      *(float*)(&data[index]) = channels[i];
      index += sizeof(float);     
    }
  }
  DEBUG_MSG("checksum = %d, index =  %d\n", getCheckSum(data, index), index)
  data[index] = getCheckSum(data, index);
  Serial.write(data, index+1);
}

void MoleGraph::setScanType() {
  DEBUG_MSG("setScanType\n")
  Serial.readBytes((uint8_t*)&scanType, 1);
}

void MoleGraph::setPeriod(bool f) {
  uint16_t x;
  Serial.readBytes((uint8_t*)&x, 2);
  if (f == 0) {
    period = (uint32_t)x * (1000 * TICK_PER_MS);    // nastaveni periody vzorkovani v 0.5 us [zadano v s] 
    DEBUG_MSG("setPeriod %d [s]\n", x)
  } else {
    period = (uint32_t)((1 / TIME_BASE) / x);    // nastaveni periody vzorkovani v 0.5 us [zadano v Hz] 
    DEBUG_MSG("setFreq = %d [Hz]\n", x)
  }
  DEBUG_MSG("period = %d [0,5us]\n", period)
}

void MoleGraph::enableChannels() {
  DEBUG_MSG("setEnableChannels\n")
  Serial.readBytes((uint8_t*)&enabledChannels, 1);
  channelCount = 0;
  for (byte i = 0; i < MAX_CHANNELS; i++) {
    if (0 != (enabledChannels & (1 << i))) channelCount++; 
  }  
}

void MoleGraph::setSendingCallback(void (*function)(void)) {
  updateFunction = function;
}

void MoleGraph::setMeasurementStartedCallback(void (*function)(void)) {
  measurementStartedCallback = function;
}

void MoleGraph::setMeasurementStoppedCallback(void (*function)(void)) {
  measurementStoppedCallback = function;
}

void MoleGraph::setMeasurementPausedCallback(void (*function)(void)) {
  measurementPausedCallback = function;
}

void MoleGraph::setMeasurementContinuedCallback(void (*function)(void)) {
  measurementContinuedCallback = function;
}
