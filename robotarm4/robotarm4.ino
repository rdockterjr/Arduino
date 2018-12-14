#include <Servo.h>

Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;
Servo servo5;
Servo servo6;
Servo servoEE;

int LED = 8;
int BTN = 2;

int defaultpos1 = 80;
int defaultpos2 = 60;
int defaultpos3 = 70;
int defaultpos4 = 95;
int defaultpos5 = 105;
int defaultpos6 = 90;
int defaultposEE = 5;
int pos1 = defaultpos1;
int pos2 = defaultpos2;
int pos3 = defaultpos3;
int pos4 = defaultpos4;
int pos5 = defaultpos5;
int pos6 = defaultpos6;
int posEE = defaultposEE;

int max_pos1 = 180;
int max_pos2 = 120;
int max_pos3 = 180;
int max_pos4 = 180;
int max_pos5 = 180;
int max_pos6 = 180;
int max_posEE = 85;
int min_pos1 = 0;
int min_pos2 = 0;
int min_pos3 = 0;
int min_pos4 = 0;
int min_pos5 = 0;
int min_pos6 = 0;
int min_posEE = 0;

//vars for Serial packet recieve
int receivePacket();
#define SOP '<'
#define EOP '>'
boolean started = false;
boolean ended = false;
char inData[80];
byte index;

//Vars to process packet
int newData[7] = {0};
char * val;
int count = 0;

void setup() {

  servo1.attach(3);
  servo2.attach(5);
  servo3.attach(6); 
  servo4.attach(9); 
  servo5.attach(10);
  servo6.attach(11);
  servoEE.attach(4);
  
  pinMode(LED,OUTPUT);
  pinMode(BTN,INPUT_PULLUP);

  Serial.begin(9600);
  Serial.println("Ready");
  
  servo1.write(defaultpos1);   
  servo2.write(defaultpos2);   
  servo3.write(defaultpos3);   
  servo4.write(defaultpos4);   
  servo5.write(defaultpos5);   
  servo6.write(defaultpos6);
  servoEE.write(defaultposEE); 
}

void loop() {
  
  int done = receivePacket();
   //<68,28,89,0,30,68>
   //<90,90,90,90,90,90>
   //<0,0,0,0,0,0>
   //<124,125,16,123,63,235>
  if(done == 1){
    pos1 = newData[0];
    pos2 = newData[1];
    pos3 = newData[2];
    pos4 = newData[3];
    pos5 = newData[4];
    pos6 = newData[5];
    posEE = newData[6];
    delay(10);
    pos1=pos1;
    pos2=pos2;
    pos3=pos3;
    pos4=pos4;
    pos5=pos5;
    pos6=pos6;
    posEE=posEE;
    Serial.println('G');
  }
  else{
    //Serial.println('B');
  }
  
  //Keep pos in bounds
  if(pos1 > max_pos1){
   pos1 = max_pos1; 
    Serial.println('L');
  }
  if(pos1 < min_pos1){
   pos1 = min_pos1; 
    Serial.println('L');
  }
  if(pos2 > max_pos2){
   pos2 = max_pos2; 
    Serial.println('L');
  }
  if(pos2 < min_pos2){
   pos2 = min_pos2; 
    Serial.println('L');
  }
  if(pos3 > max_pos3){
   pos3 = max_pos3; 
    Serial.println('L');
  }
  if(pos3 < min_pos3){
   pos3 = min_pos3; 
    Serial.println('L');
  }
  if(pos4 > max_pos4){
   pos4 = max_pos4; 
    Serial.println('L');
  }
  if(pos4 < min_pos4){
   pos4 = min_pos4; 
    Serial.println('L');
  }
  if(pos5 > max_pos5){
   pos5 = max_pos5; 
    Serial.println('L');
  }
  if(pos5 < min_pos5){
   pos5 = min_pos5; 
    Serial.println('L');
  }
  if(pos6 > max_pos6){
   pos6 = max_pos6; 
    Serial.println('L');
  }
  if(pos6 < min_pos6){
   pos6 = min_pos6; 
    Serial.println('L');
  }
  if(posEE > max_posEE){
   posEE = max_posEE; 
    Serial.println('L');
  }
  if(posEE < min_posEE){
   posEE = min_posEE; 
    Serial.println('L');
  }
  
  servo1.write(pos1);  
  servo2.write(pos2);
  servo3.write(pos3);
  servo4.write(pos4);
  servo5.write(pos5);
  servo6.write(pos6);
  servoEE.write(posEE); 
}

int receivePacket(){
 //receive data packet
  while(Serial.available() > 0)
  {
    char inChar = Serial.read();
  		
    if(inChar == SOP)
    {
      index = 0;
      inData[index] = '\0';
      started = true;
      ended = false;
    }
    else if(inChar == EOP)
    {
      ended = true;
      break;
    }
    else
    {
      if(index < 79)
      {
        inData[index] = inChar;
        index++;
        inData[index] = '\0';
      }
    }
  }
  // We are here either because all pending serial
  // data has been read OR because an end of
  // packet marker arrived. Which is it?
  if(started && ended)
  {
    val = strtok (inData,",");
		
    newData[count] = atoi(val);
		
    while ((val = strtok (NULL, ",")) != NULL)
    {
      newData[++count] = atoi(val);
    }
		
    // Reset for the next packet
    started = false;
    ended = false;
    index = 0;
    inData[index] = '\0';
    count = 0;
    
    Serial.flush();

    
    return 1;
  }
  return 0;
}
