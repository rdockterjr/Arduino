
//Needs this encoder library
//http://www.pjrc.com/teensy/td_libs_Encoder.html
#include <Encoder.h>

long oldLeft  = 0;
long oldRight = 0;
long newLeft, newRight, diffLeft, diffRight;

//Encoders 
Encoder EncMotorL(3,5);

void setup() {
  //serial for debug
  Serial.begin(9600);

  //get starting encoder
  oldLeft = EncMotorL.read();
}

void loop() {
  // put your main code here, to run repeatedly:
  newLeft = EncMotorL.read();
  diffLeft = newLeft - oldLeft;

  Serial.print("counts: ");
  Serial.println(diffLeft);

  delay(300);
}
