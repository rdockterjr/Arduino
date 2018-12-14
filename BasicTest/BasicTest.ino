#include <ctype.h>
unsigned long timer = 0;
int seconds = 0;
unsigned long difference = 0;
unsigned long timestart;
void setup()
{
  
  Serial.begin(9600);
  Serial.println("What is up"); 
  pinMode(12,OUTPUT);
  pinMode(3,INPUT_PULLUP);
  pinMode(11,OUTPUT);
  timestart = millis();
}
unsigned long timemil;
double timesec;
int breaker = 500;
int onoff = 0;
void loop()
{
  //digitalWrite(2,HIGH);
  delay(100);
  unsigned long timeend = millis() - timestart;
  int buttonval = digitalRead(3);
  int readval1 = analogRead(0);
  int readval2 = analogRead(1);
  Serial.println(readval1);
Serial.println(readval2);
if(buttonval == 0){
 digitalWrite(12,HIGH); 
}
if(buttonval == 1){
 digitalWrite(12,LOW); 
}

if(timeend >= breaker){
      breaker += 500;
      if(onoff == 0){
        digitalWrite(11,LOW);
        onoff = 1;
      }
      else{
        digitalWrite(11,HIGH);
        onoff = 0;
      }
    }
//  timemil = millis();
//  timesec = (double) timemil;
//  timesec = floor(timesec*100.0) /100000.0;
//  Serial.println(timesec);
  
  //digitalWrite(24, LOW);

}
