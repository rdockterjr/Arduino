#include <Servo.h>

Servo servo1;
Servo servo2;
Servo servo3;

int defaultpos1 = 65;
int defaultpos2 = 70;
int defaultpos3 = 85;
int pos1 = defaultpos1;
int pos2 = defaultpos2;
int pos3 = defaultpos3;

int max_pos1 = 180;
int max_pos2 = 180;
int max_pos3 = 180;
int min_pos1 = 0;
int min_pos2 = 0;
int min_pos3 = 0;

//vars for Serial packet recieve
int receivePacket();
#define SOP '<'
#define EOP '>'
boolean started = false;
boolean ended = false;
char inData[80];
byte index;

//Vars to process packet
int newData[3] = {0};
char * val;
int count = 0;

void setup() {

  servo1.attach(3);
  servo2.attach(4);
  servo3.attach(5); 

  Serial.begin(9600);
  Serial.println("Ready");
  
  servo1.write(defaultpos1);   
  servo2.write(defaultpos2);   
  servo3.write(defaultpos3);   
}

void loop() {
  
  int done = receivePacket();

  if(done == 1){
    pos1 = newData[0];
    pos2 = newData[1];
    pos3 = newData[2];
    delay(10);
    pos1= pos1 + defaultpos1;
    pos2= pos2 + defaultpos2;
    pos3= pos3 + defaultpos3;
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
  
  servo1.write(pos1);  
  servo2.write(pos2);
  servo3.write(pos3);
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
