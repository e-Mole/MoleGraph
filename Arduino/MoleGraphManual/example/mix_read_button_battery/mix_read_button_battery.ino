//********** MoleGraphManual ***************
//   MoleGraph Probeware example code
//    (www.e-mole.cz/diy/molegraph) 
//******************************************

// Enable MoleGraph U01 shield function
#define SYSTEM
#include <molegraphmanual.h>

MoleGraphManual moleGraph;

int angle = 0;
int last_angle = 0;

void updateGraphChannels(void) {
  int raw = analogRead(PORT_2A);
  int dig = digitalRead(PORT_2D);
  int butt_pressed = moleGraph.getButton(1);
  int batt_status = moleGraph.getBattery();
  
  moleGraph.setChannelValue(1, raw);
  moleGraph.setChannelValue(2, dig);
  moleGraph.setChannelValue(3, butt_pressed);
  moleGraph.setChannelValue(4, batt_status);  
}

void setup() {    
  moleGraph.init();
  moleGraph.setSendingCallback(&updateGraphChannels);
  pinMode(11, OUTPUT); //OUTPUT for tone generator
  pinMode(A0, OUTPUT);
//  SERVO.attach(9);
}

void loop() {
  moleGraph.process();  
  int val = analogRead(A1);
    if (val < 200) {
      tone(11,620);
      digitalWrite(A0, HIGH);
      angle = 90;
    } else {
      noTone(11);
      digitalWrite(A0, LOW);
      angle = 10;
    }

//    if (last_angle != angle) {
//      SERVO.write(angle);
//      last_angle = angle;      
//    }
}
