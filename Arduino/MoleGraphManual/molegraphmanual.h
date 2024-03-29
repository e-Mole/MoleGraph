#ifndef MoleGraphManual_h
#define MoleGraphManual_h

// Uncomment to enable printing out nice debug messages.
//#define DEBUG

#include <Arduino.h>
#include "timer.h"

#define VERSION       "ATG_4" //arduino to graph version
#define MAX_CHANNELS  8
#define PERIOD        (1000 * TICK_PER_MS)   

  //Used in Blockly@rduino visual blocks (with or without MoleGraph shield)
  #define PORT_1A 14
  #define PORT_1D 11
  #define PORT_1U 7
  #define PORT_2A 15
  #define PORT_2D 10
  #define PORT_2U 6
  #define PORT_3A 16
  #define PORT_3D 9
  #define PORT_3U 5
  #define PORT_4A 19
  #define PORT_4D 8
  #define PORT_4U 4

#ifdef SYSTEM
  #include "hw.h"
  System runtem; 

  #define MAX_PORTS 4
  /*
  #define PORT_1A 14
  #define PORT_1D 11
  #define PORT_1U 7
  #define PORT_2A 15
  #define PORT_2D 10
  #define PORT_2U 6
  #define PORT_3A 16
  #define PORT_3D 9
  #define PORT_3U 5
  #define PORT_4A 19
  #define PORT_4D 8
  #define PORT_4U 4
  */
  const uint8_t PINS_DIGITAL[] = {PORT_1D, PORT_2D, PORT_3D, PORT_4D};
  const uint8_t PINS_ANALOG[]  = {PORT_1A, PORT_2A, PORT_3A, PORT_4A};
  const uint8_t PINS_PULLUP[]  = {PORT_1U, PORT_2U, PORT_3U, PORT_4U};
#endif

enum Instructions {
  INS_NONE = 0,
  INS_GET_VERSION = 1,
  INS_SET_TIME = 2,
  INS_SET_FREQUENCY = 3,
  INS_ENABLED_CHANNELS = 4,
  INS_START = 5,
  INS_STOP = 6,
  INS_SET_TYPE = 7,
  INS_GET_SAMPLE = 8,
  INS_PAUSE = 9,
  INS_CONTINUE = 10,
  INS_INITIALIZE = 11,
  INS_SET_SENSOR = 12,
  INS_DEBUG = 127,
};

enum ScanType {
  PERIODICAL = 0,
  ONDEMAND = 1
};

class MoleGraphManual {
  public:
    void  init() {
      Serial.begin(115200);
      timerInit();
      #ifdef SYSTEM
        runtem.init();
      #endif
    }
    
    void  process() {
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
      #ifdef SYSTEM
        runtem.process(actualTime, measurementInProgress);
      #endif
    }

    #ifdef SYSTEM
      bool getButton(uint8_t index) {
        bool result = 0;
        if (index > 0 && index <= 4) {
          result = runtem.getBtn() & (1 << (index - 1)); 
        } 
        return result;
      }
      
      uint8_t getBattery() {
        uint8_t result = runtem.getBatt();  
        return result;
      }
            
      void setPullup(uint8_t index, bool pull) {
        if (index > 0 && index <= 4) {
          if (pull) {
            pinMode(PINS_PULLUP[index-1], OUTPUT);
            digitalWrite(PINS_PULLUP[index-1], 1);
          } else {
            pinMode(PINS_PULLUP[index-1], INPUT);
          } 
        }   
      }
    #endif
    
    bool  setChannelValue(uint8_t channel, float value);
    float getChannelValue(uint8_t channel);
    bool  isMeasurementInProgress();
    void  startMeasurement(bool restart = 0);
    void  stopMeasurement(bool pause = 0);
    
    void setSendingCallback(void (*function)(void));
    void setMeasurementStartedCallback(void (*function)(void));
    void setMeasurementStoppedCallback(void (*function)(void));
    void setMeasurementPausedCallback(void (*function)(void));
    void setMeasurementContinuedCallback(void (*function)(void));
  
  private:
    uint8_t   enabledChannels = 0;          // bitova maska aktivnich kanalu
    uint8_t   channelCount = 0;             // pocet aktivnich kanalu
//    uint8_t   enabledChannels = 1;          // bitova maska aktivnich kanalu
//    uint8_t   channelCount = 1;             // pocet aktivnich kanalu    
    float     channels[MAX_CHANNELS];       // merena data
    bool      fullWriteBufferDetected = 0;  // priznak zaplneni odesilaciho bufferu ??? TODO
    bool      sampleRequestFlag = 0;        // priznak pozadavku na mereni
    bool      measurementInProgress = 0;    // priznak spusteneho merene
    ScanType  scanType = PERIODICAL;        // priznak typu mereni - periodicke x na vyzadani
//    ScanType  scanType = ONDEMAND;        // priznak typu mereni - periodicke x na vyzadani    
    uint32_t  period = PERIOD;              // perioda v 0,5us
    uint32_t  time;                         // cas posledniho skenu
    uint32_t  startTime;                    // cas spusteni mereni
    uint32_t  actualTime;                   // aktualni cas

    void    update();                       // mereni (vola updateFunction)
    void    setPeriod(bool);                // nastaveni periody mereni 0 - perioda [s], 1 - frekvence [Hz]
    void    setScanType();                  // nastaveni typu mereni
    void    sampleRequest();
//    void    startMeasurement(bool restart = 0);
//    void    stopMeasurement(bool pause = 0);
    void    enableChannels();
    void    sendData();                     // odeslani dat
    void    sendCommand(uint8_t command, const char *format_msg, ...);
    uint8_t writeHeader(uint8_t);
    uint8_t getCheckSum(uint8_t*, uint8_t);

    void    (*updateFunction)(void);
    void    (*measurementStartedCallback)(void);
    void    (*measurementStoppedCallback)(void);
    void    (*measurementPausedCallback)(void);
    void    (*measurementContinuedCallback)(void);
};

extern MoleGraphManual moleGraph;

// Setup debug printing macros.
#ifdef DEBUG
  #define DEBUG_MSG(MESSAGE...) { sendCommand(INS_DEBUG,  MESSAGE); }
#else
  #define DEBUG_MSG(MESSAGE, ...) {}
#endif

#endif