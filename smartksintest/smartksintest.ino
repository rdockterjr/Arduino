#include <Servo.h>

float x = 1.0;
float y = 1.0;
float rad = 5.0;

int state = 1;

void setup() {

  Serial.begin(9600);
  Serial.println("Ready");
  
}

void loop() {
  
  if(state == 1){
    x = x + 1.0;
    if(x == 150.0){
     rad = 2.0;
     state = 2; 
    }
  }
  else if(state == 2){
   y = y + 1.0; 
   if(y == 150.0){
     rad = 4.0;
     state = 3; 
   }
  }
  else if(state == 3){
    x = x - 1.0;
    if(x == 1.0){
      rad = 3.0;
     state = 4; 
   }
  }
  else if(state == 4){
    y = y - 1.0;
    if(y == 1.0){
     rad = 5.0;
     state = 1; 
   }
  }
  
  Serial.print(x);
  Serial.print(',');
  Serial.print(y);
  Serial.print(',');
  Serial.println(rad);
  
  delay(100);

}

