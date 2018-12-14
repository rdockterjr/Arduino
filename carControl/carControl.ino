#include <SoftwareSerial.h>

int pin_rb = 3;
int pin_lb = 4;
int pin_lf = 5;
int pin_rf = 6;

SoftwareSerial mySerial(7, 8); // RX, TX

void setup() {
  // put your setup code here, to run once:
  
  pinMode(pin_rb, OUTPUT);
  pinMode(pin_lb, OUTPUT);
  pinMode(pin_lf, OUTPUT);
  pinMode(pin_rf, OUTPUT);
  
  mySerial.begin(9600);
  
  mySerial.println("YAY CARS");

  stops();
}

void loop() {
  // put your main code here, to run repeatedly:
  
  while (mySerial.available()) {
    delay(3);  //delay to allow buffer to fill 
    if (mySerial.available() >0) {
      char c = mySerial.read();  //gets one byte from serial buffer

      if(c == 'f'){
        forward();
      }
      else if(c == 's'){
        stops();
      }
      else if(c == 'l'){
        left();
      }
      else if(c == 'r'){
        right();
      }
    } 
  }
}


void forward(){
  mySerial.println("FORWARD!");
  digitalWrite(pin_lb,HIGH);
  digitalWrite(pin_rb,HIGH);
  digitalWrite(pin_lf,HIGH);
  digitalWrite(pin_rf,HIGH);
}

void stops(){
  mySerial.println("STOP IT!");
  digitalWrite(pin_lb,LOW);
  digitalWrite(pin_rb,LOW);
  digitalWrite(pin_lf,LOW);
  digitalWrite(pin_rf,LOW);
}

void left(){
  mySerial.println("LEANORD!");
  digitalWrite(pin_lb,LOW);
  digitalWrite(pin_rb,HIGH);
  digitalWrite(pin_lf,LOW);
  digitalWrite(pin_rf,HIGH);
}

void right(){
  mySerial.println("RODGER!");
  digitalWrite(pin_lb,HIGH);
  digitalWrite(pin_rb,LOW);
  digitalWrite(pin_lf,HIGH);
  digitalWrite(pin_rf,LOW);
}
