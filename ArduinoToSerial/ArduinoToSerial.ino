#define VERSION "ATG_1" //arduino to graph version 1
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
unsigned g_requiredTime = 0;
unsigned g_currentTime = 0;
float g_channel0 = 0;
float g_channel1 = -100;
float g_channel2 = 10;
float g_channel3 = 20;
float g_channel4 = 30;
float g_channel5 = 40;
float g_channel6 = 50;
float g_channel7 = 60;

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
   
    if (0 != (g_enabledChannels & 0x01)) 
      writeValue(0, g_channel0);
    if (0 != (g_enabledChannels & 0x02)) 
      writeValue(1, g_channel1);
    if (0 != (g_enabledChannels & 0x04)) 
      writeValue(2, g_channel2);
    if (0 != (g_enabledChannels & 0x08)) 
      writeValue(3, g_channel3); 
    if (0 != (g_enabledChannels & 0x10)) 
      writeValue(4, g_channel4);
    if (0 != (g_enabledChannels & 0x20)) 
      writeValue(5, g_channel5);
    if (0 != (g_enabledChannels & 0x40))
      writeValue(6, g_channel6);
    if (0 != (g_enabledChannels & 0x80))
      writeValue(7, g_channel7);
}

ISR(TIMER1_COMPA_vect)          // timer compare interrupt service routine
{
   if (0 != g_requiredTime && (++g_currentTime) != g_requiredTime)
    return;

  g_currentTime = 0;
  
  SendData();
}


void writeValue(unsigned char channel, float value)
{
   Serial.write(channel);
   Serial.write((char *)&value, 4);
}

void DemoUpdate()
{
  g_channel0 += 0.5;
  if (g_channel0 > 20)
  {
    g_channel0 = 0;
    g_channel1 = (-100 == g_channel1) ? 100 : -100;
  } 
  g_channel2 = 1.0 / (float)(++counter);
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
    break;
    case INS_START:
      SendData(); //first delivery must be done immediatly
      g_currentTime = 0;
      TIMSK1 |= (1 << OCIE1A);  // enable timer compare interrupt
    break;
    case INS_STOP:
      TIMSK1 &= ~(1 << OCIE1A);  // enable timer compare interrupt
    break;
    }
  }
}
