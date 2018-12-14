
//Needs this encoder library
//http://www.pjrc.com/teensy/td_libs_Encoder.html
#include <Encoder.h>

long oldLeft  = 0;
long oldRight = 0;
long newLeft, newRight, diffLeft, diffRight;

//Encoders 
Encoder EncMotorL(2,4);
Encoder EncMotorR(3,5);

void setup() {
  //serial for debug
  Serial.begin(9600);

  //get starting encoder
  oldLeft = EncMotorL.read();
  oldRight = EncMotorR.read();
}

void loop() {
  // put your main code here, to run repeatedly:
  newLeft = EncMotorL.read();
  newRight = EncMotorR.read();
  diffLeft = newLeft - oldLeft;
  diffRight = newRight - oldRight;

  Serial.print("Left: ");
  Serial.print(diffLeft);
  Serial.print(" Right: ");
  Serial.println(diffRight);

  delay(100);
}
