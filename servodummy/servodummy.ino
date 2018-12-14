#include <Servo.h>

Servo servo1;


int LED = 8;
int BTN = 2;

int pos1 = 0;

int minp = 70;
int maxp = 115;

void setup() {

  servo1.attach(9);
  
  pinMode(LED,OUTPUT);
  pinMode(BTN,INPUT_PULLUP);

  Serial.begin(9600);
  Serial.println("Ready");
  

}

void loop() {
  
  
  for(pos1 = minp; pos1 < maxp; pos1++){
    servo1.write(pos1);  
    delay(50);
  }
  delay(500);
  for(pos1 = maxp; pos1  > minp; pos1--){
    servo1.write(pos1);  
    delay(50);
  }
  delay(500);
   
}
