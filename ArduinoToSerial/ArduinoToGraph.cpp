#include "ArduinoToGraph.h"


#define VERSION "ATG_1" //arduino to graph version
#define MESSAGE_SIZE 1 + sizeof(float)

namespace
{
  enum Type
  {
      typePeriodical = 0,
      typeOnRequest = 1
  } g_type = typePeriodical;
  
  unsigned char g_enabledChannels = 0;
  unsigned char g_channelCount = 0;
  unsigned g_requiredTime = 0;
  unsigned g_currentTime = 0;
  float g_channels[8];
  bool g_fullWriteBufferDetected = false;
float g_timeFromStart = 0;
bool g_sampleRequest = false;
  
  void (*g_updateFunction)(void);
  
  void InitTimer()
  {
    noInterrupts();           // disable all interrupts
    TCCR1A = 0;
    TCCR1B = 0;
    TCNT1  = 0;
  
    TCCR1B |= (1 << WGM12);   // CTC mode
    TCCR1B |= (1 << CS12);    // 256 prescaler 
    interrupts();             // enable all interrupts
  }
  
  void WriteValue(unsigned char channel, float value, bool isCommandResponse, bool writingDelay, bool containsTimestamp)
  {
    unsigned char mixture = channel;
    mixture |= isCommandResponse << 7;
    mixture |= writingDelay << 6;
    
    Serial.write(mixture);
    Serial.write((char *)&value, sizeof(float));
  }

  void SendData(bool timestamp)
  {
    //it can happen when user set to high frequency or too many channels
    //number of data is then higher then baud rate.
    // I try to check Serial.availableForWrite() < g_channelCount * MESSAGE_SIZE but it happend always
    // the buffer is probably less then 40 and => I could 
    
    bool bufferIsFull = (Serial.availableForWrite() < g_channelCount * MESSAGE_SIZE + (timestamp ? sizeof(float) : 0)); 
  
    g_fullWriteBufferDetected |= bufferIsFull;
    
    //RX LED satay to light but application was not able to read all the data 
    //when I try to send data when I try to write data in this moment.
    //When a new messurment was started, data from previous mesurment was delivered WTF?
    
    if (bufferIsFull)
      return; //have to throw data form this sample :(

    if (timestamp)
      Serial.write((char *)&timestamp, sizeof(float));
    for (int i = 0; i < 8; i++)
    {
      if (0 != ((g_enabledChannels >> i) & 1)) 
        WriteValue(i, g_channels[i], false, g_fullWriteBufferDetected, false); 
    }
  
     g_fullWriteBufferDetected = false;
  }

  void FillFromSerial(unsigned char &value)
  {
    while (0 == Serial.available())
    {}
    value = Serial.read();
  }

  void FillFromSerial(unsigned &value)
  {
    while (0 == Serial.available())
    {}
    value = Serial.read();
    while (0 == Serial.available())
    {}
    value |= Serial.read() << 8;
  }

} //namespace


ISR(TIMER1_COMPA_vect)          // timer compare interrupt service routine
{
  if (g_type == typeOnRequest)
  {
    g_timeFromStart += 1/62500;
    if (!g_sampleRequest)  
      return;

     g_sampleRequest = false;
  }
  else //type periodical
  {
    if (0 != g_requiredTime && (++g_currentTime) != g_requiredTime)
      return;
    g_currentTime = 0;
  }
    
  g_updateFunction();  
  SendData(g_type == typeOnRequest);
}

void ArtuinoToGraph::Setup(float channel1, float channel2, float channel3, float channel4, float channel5, float channel6, float channel7, float channel8)
{
  g_channels[0] = channel1;
  g_channels[1] = channel2;
  g_channels[2] = channel3;
  g_channels[3] = channel4;
  g_channels[4] = channel5;
  g_channels[5] = channel6;
  g_channels[6] = channel7;
  g_channels[7] = channel8;
  
  Serial.begin(115200);
  while (!Serial) 
  {}

  InitTimer();  
}

void ArtuinoToGraph::Loop()
{
  if (0 != Serial.available())
  {
    unsigned char instruction = Serial.read();
    switch (instruction) 
    {
    case INS_GET_VERSION:
      Serial.write(VERSION);
    break;
    case INS_SET_FREQUENCY:
    {
      unsigned frequency;
      FillFromSerial(frequency);
      
      InitTimer(); //workaround there was a 1s lag after start when there had been set 1 Hz period and user set 200 Hz period   
      OCR1A = 62500/frequency;            // compare match register 16MHz/256/2Hz
    }
    break;
    case INS_SET_TIME:
    {
      FillFromSerial(g_requiredTime);

      InitTimer(); //workaround as in set frequenc case
      OCR1A = 62500;            // compare match register 16MHz/256/2Hz
    }
    break;
    case INS_ENABLED_CHANNELS:
      FillFromSerial(g_enabledChannels);
      g_channelCount = 0;
      for (int i = 0; i < 8; i++)
        if (0 != (g_enabledChannels & (1 << i)))
          g_channelCount++;       
    break;
    case INS_START:
      g_fullWriteBufferDetected = false;
      TIMSK1 |= (1 << OCIE1A);  // enable timer compare interrupt
    break;
    case INS_STOP:
      TIMSK1 &= ~(1 << OCIE1A);  // disable timer compare interrupt
    break;
    case INS_TYPE:
      unsigned char type;
      FillFromSerial(type);
      g_type = (Type)type;
      if (g_type == typeOnRequest)
      {
        g_sampleRequest = false;
        g_timeFromStart = 0;
        OCR1A = 1; //1/62500 s
      }
      else
      {
        g_currentTime = g_requiredTime - 1; //to be data send immediately
      }
    break;
    }
  }
}

bool ArtuinoToGraph::SetChannelValue(int channel, float value)
{
  if (channel < 1 && channel > 8)
    return false;
    
  g_channels[channel - 1] = value; //uses count channels from 1 
  
  return true;
}

float ArtuinoToGraph::GetChannelValue(int channel)
{
  if (channel < 1 && channel > 8)
    return 0;
    
  return g_channels[channel - 1];
}

void ArtuinoToGraph::SetUpdateCallbackFunction(void (*f)(void) )
{
  g_updateFunction = f;
}

void ArtuinoToGraph::SampleRequest()
{
  g_sampleRequest = true;
}
