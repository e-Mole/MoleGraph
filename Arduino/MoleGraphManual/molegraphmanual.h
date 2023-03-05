#ifndef MoleGraphManual_h
#define MoleGraphManual_h

#include "hw.h"

// Uncomment to enable printing out nice debug messages.
//#define DEBUG

#define VERSION       "ATG_4" //arduino to graph version - nutno změnit na něco jako ATG_4_1
#define MAX_CHANNELS  8
#define PERIOD        (1000 * TICK_PER_MS)   

#define MAX_PORTS 4

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

enum Instructions
{
  INS_NONE = 0,
  INS_GET_VERSION = 1,
//  INS_GET_VERSION = 'v',
  INS_SET_TIME = 2,
  INS_SET_FREQUENCY = 3,
  INS_ENABLED_CHANNELS = 4,
  INS_START = 5,
  INS_STOP = 6,
//  INS_START = 's',
//  INS_STOP = 'e',
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

class MoleGraph {
  public:
    void     init();
    void     process();
    bool     getButton(uint8_t); // tlačítka 1, 2, 3, 4 (vrací 0/1)
	uint8_t  getBattery(); // vraci stav 2 = ok, 1 = low alert, 0 = no battety	
    void     setPullup(uint8_t, bool pull = 1); // na portu 1, 2, 3, 4 (1 = zapnuto)
    
    bool     setChannelValue(uint8_t channel, float value);
    float    getChannelValue(uint8_t channel);
    bool     isMeasurementInProgress();
    
    void     setSendingCallback(void (*function)(void));
    void     setMeasurementStartedCallback(void (*function)(void));
    void     setMeasurementStoppedCallback(void (*function)(void));
    void     setMeasurementPausedCallback(void (*function)(void));
    void     setMeasurementContinuedCallback(void (*function)(void));
  
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
    void    startMeasurement(bool restart = 0);
    void    stopMeasurement(bool pause = 0);
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

extern MoleGraph moleGraph;

// Setup debug printing macros.
#ifdef DEBUG
  #define DEBUG_MSG(MESSAGE...) { sendCommand(INS_DEBUG,  MESSAGE); }
#else
  #define DEBUG_MSG(MESSAGE, ...) {}
#endif

#endif
