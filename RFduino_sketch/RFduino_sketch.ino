#include <SoftwareSerial.h>

SoftwareSerial mySerial(5, 6); // RX, TX


#define HORZL   3
#define VERTL   2
#define SELL   8
#define HORZR   4
#define VERTR   5
#define SELR   3

int lv_val = 0;
int lh_val = 0;
int l_sel = 0;
int rv_val = 0;
int rh_val = 0;
int r_sel = 0;


void setup() {
  
  // Enable serial debug, type cntrl-M at runtime.
  Serial.begin(9600);
  Serial.println("ArduPad started");
  
  // set the data rate for the SoftwareSerial port
  mySerial.begin(9600);
  
  //buttons
  pinMode(SELL,INPUT_PULLUP);
  pinMode(SELR,INPUT_PULLUP);
}

int directionLogic(int horzR, int vertR, int horzL, int vertL){
  //returns 0 - 8 depending on joystick
  int noiseThresh = 100;
  
  int absborzR = abs(horzR - 512);
  int absvertR = abs(vertR - 512);
  int absborzL = abs(horzL - 512);
  int absvertL = abs(vertL - 512);
  if(absvertR > 100 && absvertR > absborzR && absvertR > absborzL && absvertR > absvertL ){
    //Forward(1) or backward(2)
    if(vertR - 512 > 0){
      return 1;
    } else{
      return 2;
    }
  }
  if(absborzR > 100 && absborzR > absvertR && absborzR > absborzL && absborzR > absvertL ){
    //Right(4) or Left(3)
    if(horzR - 512 > 0){
      return 3;
    } else{
      return 4;
    }
  }
  if(absborzL > 100 && absborzL > absborzR && absborzL > absvertR && absborzL > absvertL ){
    //clockwise (6) or counterclock (5)
    if(horzL - 512 > 0){
      return 5;
    } else{
      return 6;
    }
  }
  if(absvertL > 100 && absvertL > absborzR && absvertL > absvertR && absvertL > absborzL ){
    //bowdown (7) or bowup (8)
    if(vertL - 512 > 0){
      return 7;
    } else{
      return 8;
    }
  }
  
  return 0;
  
}

int potSpeed(int horzR, int vertR, int horzL, int vertL, int directionVS){
  //returns speed based on direction logic
  int offset = 612;
  
  if(directionVS == 1 || directionVS == 2){
    return abs(vertR - offset);
  }
  if(directionVS == 3 || directionVS == 4){
    return abs(horzR - offset);
  }
  if(directionVS == 5 || directionVS == 6){
    return abs(horzL - offset);
  }
  if(directionVS == 7 || directionVS == 8){
    return abs(vertL - offset);
  }
  
  return 0;
  
}

void sendRawVals(){
  //send all raw pot vals
  lv_val = analogRead(VERTL);
  lh_val = analogRead(HORZL);
  rv_val = analogRead(VERTR);
  rh_val = analogRead(HORZR);
  l_sel = digitalRead(SELL);
  r_sel = digitalRead(SELR);
  
  mySerial.print('<');
  mySerial.print(lv_val);
  mySerial.print(',');
  mySerial.print(lh_val);
  mySerial.print(',');
  mySerial.print(rv_val);
  mySerial.print(',');
  mySerial.print(rh_val);
  mySerial.print(',');
  mySerial.print(l_sel);
  mySerial.print(',');
  mySerial.print(r_sel);
  mySerial.println('>');
}


void loop() {
  //run through all vals and get em
  lv_val = analogRead(VERTL);
  lh_val = analogRead(HORZL);
  rv_val = analogRead(VERTR);
  rh_val = analogRead(HORZR);
  l_sel = digitalRead(SELL);
  r_sel = digitalRead(SELR);
  
  int what2do = directionLogic(rh_val, rv_val, lh_val, lv_val);
  int speed2do = potSpeed(rh_val, rv_val, lh_val, lv_val, what2do);
    
  mySerial.print('<');
  mySerial.print(what2do);
  mySerial.print(',');
  mySerial.print(speed2do);
  mySerial.println('>');
  
  delay (30);
}
