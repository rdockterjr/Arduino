#include <Servo.h>

Servo servo1;
Servo servo2;

int LED = 4;

int defaultpos1 = 90;
int defaultpos2 = 90;
int pos1 = defaultpos1;
int pos2 = defaultpos2;

int max_pos1 = 180;
int max_pos2 = 180;
int min_pos1 = 0;
int min_pos2 = 0;

double scale1 = 0.015;
double scale2 = 0.015;

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

  servo1.attach(2);
  servo2.attach(3);
  
  pinMode(LED,OUTPUT);

  Serial.begin(9600);
  Serial.println("Ready");
  
  servo1.write(defaultpos1);   
  servo2.write(defaultpos2);   
}

void loop() {
  
  int done = receivePacket();
   //<68,28,89,0,30,68>
   //<90,90,90,90,90,90>
   //<0,0,0,0,0,0>
   //<124,125,16,123,63,235>
  if(done == 1){
    int error1 = newData[0];
    int error2 = newData[1];

    pos1=pos1+int(double(double(error1)*scale1));
    pos2=pos2-int(double(double(error2)*scale2));

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

  
  servo1.write(pos1);  
  servo2.write(pos2);
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
