#include <Servo.h>

Servo myservo;

int pos = 0;


void setup() {

  myservo.attach(8); //analog pin 0

  Serial.begin(9600);
  Serial.println("Ready");

}

void loop() {
  
   if ( Serial.available()) {
    pos = Serial.parseInt(); // important .Read() only does chars

    if(pos>180){
     pos = 180; 
    }
    if(pos<1){
     pos = 1; 
    }
    Serial.println(pos);
    myservo.write(pos);              // tell servo to go to position in variable 'pos' 
    delay(15); 

   }

//  for(pos = 0; pos < 180; pos += 1)  // goes from 0 degrees to 180 degrees 
//  {                                  // in steps of 1 degree 
//    myservo.write(pos);              // tell servo to go to position in variable 'pos' 
//    delay(15);                       // waits 15ms for the servo to reach the position 
//  } 
//  for(pos = 180; pos>=1; pos-=1)     // goes from 180 degrees to 0 degrees 
//  {                                
//    myservo.write(pos);              // tell servo to go to position in variable 'pos' 
//    delay(15);                       // waits 15ms for the servo to reach the position 
//  }

//  myservo.write(30);              // tell servo to go to position in variable 'pos' 
//  delay(500);
//  myservo.write(100);              // tell servo to go to position in variable 'pos' 
//  delay(500);

} 
