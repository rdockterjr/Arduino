#include <Servo.h>

int input;

int LED = 8;

void setup() {

  Serial.begin(9600);
  Serial.println("Ready");
  
  pinMode(LED,OUTPUT);
}

void loop() {
  
   if ( Serial.available()) {
    input = Serial.parseInt();
    if(input == 100){
      digitalWrite(LED,HIGH);
    }
    if(input == 294){
      digitalWrite(LED,LOW);
    }
   }
}
