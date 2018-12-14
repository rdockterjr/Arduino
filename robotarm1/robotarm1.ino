#include <Servo.h>

Servo servo1;
Servo servo2;
Servo servo3;

int LED = 8;
int BTN = 2;

int pos1 = 90;
int pos2 = 90;
int pos3 = 30;
int defaultpos1 = pos1;
int defaultpos2 = pos2;
int defaultpos3 = pos3;
char input;

void setup() {

  servo1.attach(11); //analog pin 0
  servo2.attach(10); //analog pin 0
  servo3.attach(9); //analog pin 0
  
  pinMode(LED,OUTPUT);
  pinMode(BTN,INPUT_PULLUP);

  Serial.begin(9600);
  Serial.println("Ready");
  
  servo1.write(defaultpos1);   
  servo2.write(defaultpos2);   
  servo3.write(defaultpos3);   

}

void loop() {
  
   if ( Serial.available()) {
    digitalWrite(LED,HIGH);
    input = Serial.read(); // important .Read() only does chars
    Serial.println(input);
    
    if(input == 119){ //w
      pos2 += 5;
      if(pos2 > 180){
       pos2 = 180; 
      }
      servo2.write(pos2);  
    }
    if(input == 115){ //s
      pos2 -= 5;
      if(pos2 < 0){
       pos2 = 0; 
      }
      servo2.write(pos2);  
    }
    if(input == 97){ //a
      pos1 += 5;
      if(pos1 > 180){
       pos1 = 180; 
      }
      servo1.write(pos1);  
    }
    if(input == 100){ //d
      pos1 -= 5;
      if(pos1 < 0){
       pos1 = 0; 
      }
      servo1.write(pos1);  
    }
    digitalWrite(LED,LOW);
   }
   
   while(digitalRead(BTN) == LOW){
     digitalWrite(LED,HIGH);
   }
   if(digitalRead(BTN) == HIGH){
     digitalWrite(LED,LOW);
   }
}

void dummyfunc(){
 //Wrist rotate

  for(pos1 = 90; pos1 <120; pos1 +=1){
    servo1.write(pos1);              // tell servo to go to position in variable 'pos' 
    delay(15); 
  }
  for(pos1 = 120; pos1 > 90; pos1 -=1){
    servo1.write(pos1);              // tell servo to go to position in variable 'pos' 
    delay(15); 
  }
  
  digitalWrite(LED,HIGH);
  delay(300);
  digitalWrite(LED,LOW);
  
  
  //wrist yaw
  
  for(pos2 = 90; pos2 <120; pos2 +=1){
    servo2.write(pos2);              // tell servo to go to position in variable 'pos' 
    delay(15); 
  }
  for(pos2 = 120; pos2 > 90; pos2 -=1){
    servo2.write(pos2);              // tell servo to go to position in variable 'pos' 
    delay(15); 
  }
  
  digitalWrite(LED,HIGH);
  delay(300);
  digitalWrite(LED,LOW);
  
  //finger rotate
  
  for(pos3 = 90; pos3 <120; pos3 +=1){
    servo3.write(pos3);              // tell servo to go to position in variable 'pos' 
    delay(15); 
  }
  for(pos3 = 120; pos3 > 90; pos3 -=1){
    servo3.write(pos3);              // tell servo to go to position in variable 'pos' 
    delay(15); 
  }
  
  digitalWrite(LED,HIGH);
  delay(300);
  digitalWrite(LED,LOW); 
  
  
}
