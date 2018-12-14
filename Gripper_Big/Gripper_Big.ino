#include <Servo.h>

Servo servoL;
Servo servoR;

//pin numbers
#define pinL 3
#define pinR 4

//servos positions
int posL = 0;
int posR = 0;
int openposL = 120;
int openposR = 70;
int closeposL = 75;
int closeposR = 115;

void setup() {

  servoL.attach(pinL);
  servoR.attach(pinR);

  //set to deafult pos
  servoL.write(openposL); 
  servoR.write(openposR);
  
  Serial.begin(9600);
  Serial.println("Ready");
 
}

void loop() {

  // send data only when you receive data:
  if (Serial.available() > 0) {
      // read the incoming byte:
      int incomingByte = Serial.parseInt();

      
      // say what you got:
      Serial.print("I received: ");
      Serial.println(incomingByte);

      if(incomingByte == 1){
        servoL.write(openposL); 
        servoR.write(openposR); 
        Serial.println("Open");
      }
      if(incomingByte == 2){
        servoL.write(closeposL); 
        servoR.write(closeposR); 
        Serial.println("Close");
      }
  }
}
