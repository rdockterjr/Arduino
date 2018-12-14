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
int currentx = 0;
int currenty = 0;
int centerx = 320;
int centery = 240;
int diffx = 0;
int diffy = 0;
float kpx = 0.02;
float kpy = 0.02;

//vars for Serial packet recieve
int receivePacket();
#define SOP '<'
#define EOP '>'
boolean started = false;
boolean ended = false;
char inData[80];
byte index;

//Vars to process packet
int newData[4] = {0};
char * val;
int count = 0;

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
  
  int done = receivePacket();
   
  if(done == 1){
    currentx = newData[0];
    currenty = newData[1];
    centerx = newData[2];
    centery = newData[3];
    delay(10);
    Serial.println('G');
  }
  else{
    Serial.println('B');
  }
  
  if(done == 1){
    diffx = currentx - centerx;
    diffy = currenty - centery;
    
    pos1 = pos1 - diffx*kpx;
    if(pos1 > 180){
     pos1 = 180; 
    }
    if(pos1 < 0){
     pos1 = 0; 
    }
      
    servo1.write(pos1);  
    
    pos2 = pos2 - diffy*kpy;
    if(pos2 > 180){
     pos2 = 180; 
    }
    if(pos2 < 0){
     pos2 = 0; 
    }
    
    servo2.write(pos2);  
  }
  done = 0;
   
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
