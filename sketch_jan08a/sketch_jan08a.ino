#include <Servo.h>


char readc;

//Define servo
Servo PenServo;
int servopin = 8;
int SERVOUP = 90;
int SERVODOWN = 60;

void setup() {
  // put your setup code here, to run once:
  pinMode(4,OUTPUT);
  digitalWrite(4,LOW);
  Serial.begin(9600);

    //Servo
  PenServo.attach(servopin);
  PenServo.write(SERVODOWN);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(Serial.available()){
    readc = Serial.read();
    if(readc == 'u'){
      digitalWrite(4,HIGH);
      PenServo.write(SERVOUP);
    }
    else if(readc == 'd'){
      digitalWrite(4,LOW);
      PenServo.write(SERVODOWN);
    }
  }
}
