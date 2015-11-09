#define VERSION "ATG_1" //arduino to graph version 1
#define MESSAGE_SIZE 5
enum Instructions
{
  INS_GET_VERSION = 1,
  INS_SET_TIME = 2,
  INS_SET_FREQUENCY = 3,
  INS_ENABLED_CHANNELS = 4,
  INS_START = 5,
  INS_STOP = 6
};

unsigned counter =  0;
unsigned char g_enabledChannels = 0;
unsigned char g_channelCount = 0;
unsigned g_requiredTime = 0;
unsigned g_currentTime = 0;
float g_channel1 = 0;
float g_channel2 = -100;
float g_channel3 = 10;
float g_channel4 = 20;
float g_channel5 = 30;
float g_channel6 = 40;
float g_channel7 = 50;
float g_channel8 = 60;
bool g_fullWriteBufferDetected = false;

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
void setup() 
{
  Serial.begin(115200);
  while (!Serial) 
  {}

  InitTimer();  
}

void SendData()
{
  UpdateChannels();
  
  unsigned bytesToSend = g_channelCount * MESSAGE_SIZE;

  //it can happen when user set to high frequency or too many channels
  //number of data is then higher then baud rate.
  // I try to check Serial.availableForWrite() < g_channelCount * MESSAGE_SIZE but it happend always
  // the buffer is probably less then 40 and => I could 
  
  bool bufferIsFull = (Serial.availableForWrite() < g_channelCount * MESSAGE_SIZE); 

  g_fullWriteBufferDetected |= bufferIsFull;
  
  //RX LED satay to light but application was not able to read all the data 
  //when I try to send data when I try to write data in this moment.
  //When a new messurment was started, data from previous mesurment was delivered WTF?
  
  if (bufferIsFull)
    return; //have to throw data form this sample :(

  bool firstNotWritten = true;
  if (0 != (g_enabledChannels & 0x01)) 
  {
    writeValue(0, g_channel1, firstNotWritten, g_fullWriteBufferDetected); 
    firstNotWritten = false;
  }
  
  if (0 != (g_enabledChannels & 0x02)) 
  {
    writeValue(1, g_channel2, firstNotWritten, g_fullWriteBufferDetected);
    firstNotWritten = false;
  }
  
  if (0 != (g_enabledChannels & 0x04)) 
  {
    writeValue(2, g_channel3, firstNotWritten, g_fullWriteBufferDetected);
    firstNotWritten = false;
  }
  
  if (0 != (g_enabledChannels & 0x08)) 
  {
    writeValue(3, g_channel4, firstNotWritten, g_fullWriteBufferDetected);
    firstNotWritten = false;
  }
  
  if (0 != (g_enabledChannels & 0x10)) 
  {
    writeValue(4, g_channel5, firstNotWritten, g_fullWriteBufferDetected);
    firstNotWritten = false;
  }
  
  if (0 != (g_enabledChannels & 0x20)) 
  {
    writeValue(5, g_channel6, firstNotWritten, g_fullWriteBufferDetected);
    firstNotWritten = false;
  }
  
  if (0 != (g_enabledChannels & 0x40))
  {
    writeValue(6, g_channel7, firstNotWritten, g_fullWriteBufferDetected);
    firstNotWritten = false;
  }
  
  if (0 != (g_enabledChannels & 0x80))
  {
    writeValue(7, g_channel8, firstNotWritten, g_fullWriteBufferDetected);
    firstNotWritten = false;
  }

   g_fullWriteBufferDetected = false;
}

ISR(TIMER1_COMPA_vect)          // timer compare interrupt service routine
{
   if (0 != g_requiredTime && (++g_currentTime) != g_requiredTime)
    return;

  g_currentTime = 0;
  
  SendData();
}


void writeValue(unsigned char channel, float value, bool firstInSample, bool writingDelay)
{
  unsigned char mixture = channel;
  mixture |= firstInSample << 7;
  mixture |= writingDelay << 6;
  
  Serial.write(mixture);
  Serial.write((char *)&value, 4);
}

void DemoUpdate()
{
  g_channel1 += 0.5;
  if (g_channel1 > 20)
  {
    g_channel1 = 0;
    g_channel2 = (-100 == g_channel2) ? 100 : -100;
  } 
  g_channel3 = 1.0 / (float)(++counter);
}
void UpdateChannels()
{
  /*
   * TODO: replace this demo by a port values reading. 
   * The graph application will be displa values which you assign to g_channelX variables 
   * therefore this place is suitable to transfer 10-bit values which ou get from A/D converter to the value which you want to display
   */ 
  DemoUpdate();
}

void loop() 
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
      while (0 == Serial.available())
      {}
      unsigned frequency = Serial.read();
      while (0 == Serial.available())
      {}
      frequency |= (Serial.read() << 8);
      g_requiredTime = 0; //there maight left a value from last run 
      
      InitTimer(); //workaround there was a 1s lag after start when there had been set 1 Hz period and user set 200 Hz period   
      OCR1A = 62500/frequency;            // compare match register 16MHz/256/2Hz
    }
    break;
    case INS_SET_TIME:
    {
      while (0 == Serial.available())
      {}
      g_requiredTime = Serial.read();
      while (0 == Serial.available())
      {}
      g_requiredTime |= (Serial.read() << 8);
      g_currentTime = g_requiredTime; 

      InitTimer(); //workaround as in set frequenc case
      OCR1A = 62500;            // compare match register 16MHz/256/2Hz
    }
    break;
    case INS_ENABLED_CHANNELS:
      while (0 == Serial.available())
      {}
      g_enabledChannels  = Serial.read();
      g_channelCount = 0;
      for (int i = 0; i < 8; i++)
        if (0 != (g_enabledChannels & (1 << i)))
          g_channelCount++;       
    break;
    case INS_START:
      g_currentTime = 0;
      g_fullWriteBufferDetected = false;
      TIMSK1 |= (1 << OCIE1A);  // enable timer compare interrupt
    break;
    case INS_STOP:
      TIMSK1 &= ~(1 << OCIE1A);  // disable timer compare interrupt
    break;
    }
  }
}
