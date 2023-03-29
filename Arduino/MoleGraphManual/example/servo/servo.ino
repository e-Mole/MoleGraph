#include <molegraphmanual.h> // Link MoleGraph library (www.e-mole.cz/diy/molegraph)

MoleGraphManual moleGraph; // Create MoleGraph instance
#define AD PORT_1A

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
  if (millis() - x >= TIME) {
    x += TIME;
    s.write(s.read() == 1000 ? 2000 : 1000);
  }
}
