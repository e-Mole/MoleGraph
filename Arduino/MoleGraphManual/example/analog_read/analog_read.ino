#include <molegraphmanual.h>

MoleGraphManual moleGraph;

//#define AD PORT_1A

//Servo SERVO;

int angle = 0;
int last_angle = 0;

void updateGraphChannels(void) {
  int raw = analogRead(PORT_2A);
  int dig = digitalRead(PORT_2D);
  moleGraph.setChannelValue(1, raw);
  moleGraph.setChannelValue(2, dig);
}

void setup() {    
  moleGraph.init();
  moleGraph.setSendingCallback(&updateGraphChannels);
  pinMode(11, INPUT); //OUTPUT for tone generator
  pinMode(A0, OUTPUT);
//  SERVO.attach(9);
}

void loop() {
  moleGraph.process();  
  int val = analogRead(A1);
    if (val < 200) {
      //tone(11,620);
      digitalWrite(A0, HIGH);
      angle = 90;
    } else {
      //noTone(11);
      digitalWrite(A0, LOW);
      angle = 10;
    }

//    if (last_angle != angle) {
//      SERVO.write(angle);
//      last_angle = angle;      
//    }
}
