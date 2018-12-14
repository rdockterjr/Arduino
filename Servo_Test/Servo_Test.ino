#include <Servo.h>

Servo servo1;

int pos = 0;
int Serial_Return_Val = 0;
boolean Serial_String_Complete = false;
int Slack_End = -90;
int Force_End = 90;

void setup() {

  servo1.attach(11); //analog pin 0

  Serial.begin(9600);
  Serial.println("Ready");
  servo1.write(45);

  pinMode(13,OUTPUT);
}

void loop() {
    for(pos = 45; pos <= 90; pos += 1) 
    {                                
      servo1.write(pos);
      delay(1);
    }
    
    delay(900);
    digitalWrite(13,HIGH);
    for(pos = 90; pos >= 45; pos -= 1) 
    {                                
      servo1.write(pos);
      delay(1);
    }
    delay(900);
    digitalWrite(13,LOW);
} 

//void serialReader(){
//  Serial_Return_Val = 0;
//  Serial_String_Complete = false;
//  int makeSerialStringPosition;
//  int inByte;
//  char serialReadString[50];
//  memset(&serialReadString[0], 0, sizeof(serialReadString));
//  const int terminatingChar = 47; //Terminate lines with '/'
//
//  inByte = Serial.read();
//  makeSerialStringPosition=0;
//
//  if (inByte > 0 && inByte != terminatingChar) { 
//    delay(100); 
//    
//    while (inByte != terminatingChar && Serial.available() > 0){
//      serialReadString[makeSerialStringPosition] = inByte; 
//      makeSerialStringPosition++; 
//      
//      inByte = Serial.read(); 
//    }
//
//    if (inByte == terminatingChar)
//    {
//      serialReadString[makeSerialStringPosition] = 0; 
//      Serial_String_Complete = true;
//      if (strcmp(serialReadString, "Force") == 0){
//        Serial_Return_Val = 1;
//      }
//      else if(strcmp(serialReadString, "Slack") == 0){
//        Serial_Return_Val = 2;
//      }
//      else{
//        Serial_Return_Val = 0;
//      }
//    }
//    Serial.println(serialReadString);
//  }
//  
//}
