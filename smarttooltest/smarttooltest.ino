#include <Servo.h>

//pin
int LED = 2;

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
int val1 = 0;
int val2 = 0;
int val3 = 0;
int val4 = 0;

void setup() {
  
  pinMode(LED,OUTPUT);
  
  Serial.begin(9600);
  Serial.println("Ready");
  
}

void loop() {
  
  int done = receivePacket();
   //<90,15,40,27>
   //<18,21,32,93>

  if(done == 1){
    val1 = newData[0];
    val2 = newData[1];
    val3 = newData[2];
    val4 = newData[3];
    delay(10);
    Serial.print("Vals: ");
    Serial.print(val1);
    Serial.print(",");
    Serial.print(val2);
    Serial.print(",");
    Serial.print(val3);
    Serial.print(",");
    Serial.println(val4);
    
    if(val1 == 90 && val2 == 15 && val3 == 40 && val4 == 27){
      digitalWrite(LED,HIGH);
    }
    if(val1 == 18 && val2 == 21 && val3 == 32 && val4 == 93){
      digitalWrite(LED,LOW);
    }
  }
  else{
    //Serial.println('B');
  }

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
