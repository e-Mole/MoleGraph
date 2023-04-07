//********** MoleGraphManual ***************
//   MoleGraph Probeware example code
//    (www.e-mole.cz/diy/molegraph) 
//******************************************

// Enable MoleGraph U01 shield function
#define SYSTEM
#include <molegraphmanual.h>

MoleGraphManual moleGraph;

void updateGraphChannels(void) {
  int raw = analogRead(PORT_2A);
  int dig = digitalRead(PORT_2D);
  moleGraph.setChannelValue(1, raw);
  moleGraph.setChannelValue(2, dig);
}

void setup() {    
  moleGraph.init();
  moleGraph.setSendingCallback(&updateGraphChannels);
  pinMode(PORT_1D, INPUT);
  pinMode(PORT_1A, OUTPUT);

}

void loop() {
  moleGraph.process();  
  int val = analogRead(PORT_2A);
    if (val < 200) {
      digitalWrite(PORT_1A, HIGH);
    } else {
      digitalWrite(PORT_1A, LOW);
    }
}
