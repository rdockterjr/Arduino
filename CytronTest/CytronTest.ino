#include <Cytron.h>

int pwmpin1 = 11;
int dirpin1 = 12;

//cytron class
Cytron cytron(pwmpin1, dirpin1, HIGH);

void setup() {
  // set up pin modes
  cytron.Init();

  Serial.begin(9600);
}

void loop() {
  if (Serial.available() > 0) {
    // read the val
    int percent = Serial.parseInt();

    // say what you got:
    Serial.print("I received: ");
    Serial.println(percent);

    cytron.Control(percent);
  }
  

}
