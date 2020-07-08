#include <AFMotor.h>




AF_DCMotor motor(1);

int speeder = 0;

void setup() {
  Serial.begin(9600);           // set up Serial library at 9600 bps
  Serial.println("Motor test!");

  // turn on motor
  motor.setSpeed(0);
  motor.run(RELEASE);
}

void loop() {

  if (Serial.available()>0)
  {      
    speeder =Serial.parseInt();
    while(Serial.available() >0){
      Serial.read();
    }
  }
  
  Serial.println(speeder);
  
  motor.run(FORWARD);
  motor.setSpeed(speeder);  
  delay(1000);
  
}
