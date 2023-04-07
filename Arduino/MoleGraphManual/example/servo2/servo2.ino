//********** MoleGraphManual ***************
//   MoleGraph Probeware example code
//    (www.e-mole.cz/diy/molegraph) 
//******************************************

// Enable MoleGraph U01 shield function
#define SYSTEM
#include <molegraphmanual.h> // Link MoleGraph library (www.e-mole.cz/diy/molegraph)

MoleGraphManual moleGraph; // Create MoleGraph instance

Servo s;

unsigned long x_time = millis();
int x_interval = 100;
int a_sensor = 0;
int s_uhel = 0;

void updateGraphChannels(void) {
  moleGraph.setChannelValue(1, a_sensor);
  moleGraph.setChannelValue(2, s_uhel);
  moleGraph.setChannelValue(3, s.read());
  moleGraph.setChannelValue(4, x_interval);
  moleGraph.setChannelValue(5, x_time);
}


void setup() {

  moleGraph.init();
  moleGraph.setSendingCallback(&updateGraphChannels);

  s.attach(2);

}

void loop() {
    moleGraph.process();
    a_sensor = analogRead(PORT_1A);
    if (millis() - x_time >= x_interval) {
      x_time = x_time + x_interval;
      s_uhel = int(map(a_sensor, 0, 1023, 500, 2500));
      s.write(s_uhel);

    }

}
