//********** MoleGraphManual ***************
//   MoleGraph Probeware example code
//    (www.e-mole.cz/diy/molegraph) 
//******************************************

// Enable MoleGraph U01 shield function
#define SYSTEM
#include "molegraphmanual.h"  // Link MoleGraph library (www.e-mole.cz/diy/molegraph)

MoleGraphManual moleGraph;

#define AD A0

Servo s;

void updateGraphChannels(void) {
  int raw = analogRead(AD);
  moleGraph.setChannelValue(1, raw);
}

void setup() {    
  moleGraph.init();
  moleGraph.setSendingCallback(&updateGraphChannels);
  s.attach(2);
}

uint32_t x = millis();
#define TIME 3000

void loop() {
  moleGraph.process();
  moleGraph.getButton(1);
  if (millis() - x >= TIME) {
    x += TIME;
    s.write(s.read() == 1000 ? 2000 : 1000);
  }
}