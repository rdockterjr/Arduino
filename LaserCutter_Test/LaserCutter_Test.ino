#include <AccelStepper.h>
#include <MultiStepper.h>

//These need to be calibrated
//(200 (step/rev) * 32 (micro/steps) ) / ( 13.9 mm (dia) * 3.14) = steps/mm
#define X_MM_2_STEPS 160 
#define Y_MM_2_STEPS 160

#define DEFAULT_SPEED 10000
#define MAX_SPEED 15000

#define MAX_X_MM 400
#define MAX_Y_MM 400

//pins
#define LEDpin 4
#define startBTN A4
#define limPinX 5
#define limPinY 6


// Define a stepper and the pins it will use
AccelStepper stepper_X(AccelStepper::DRIVER,3,2); //(DRIVER,Step,Dir)
AccelStepper stepper_Y(AccelStepper::DRIVER,12,11); //(DRIVER,Step,Dir)

//Do multi Stepper
MultiStepper stepper2D;


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


//new values for steppers
int cmd_x_mm = 0;
int cmd_y_mm = 0;
int cmd_onoff = 0; // 0 off, 1 on


//controls
bool PAUSE = false;
int PACKET = false;



void setup() 
{         
  Serial.begin(9600);
  
  //Setup Steppers (dont setSpeed(), thats managed by multistepper)
  stepper_X.setMaxSpeed(MAX_SPEED);
  stepper_Y.setMaxSpeed(MAX_SPEED);
  
  // Then give them to MultiStepper to manage
  stepper2D.addStepper(stepper_X);//array[0]
  stepper2D.addStepper(stepper_Y);//array[1]
  
  //set pin modes
  pinMode(LEDpin, OUTPUT); 
  pinMode(startBTN, INPUT_PULLUP); 
  pinMode(limPinX, INPUT_PULLUP); 
  pinMode(limPinY, INPUT_PULLUP); 

  //flash LED
  digitalWrite(LEDpin,HIGH);
  delay(250);  
  digitalWrite(LEDpin,LOW);
  delay(250);  
  digitalWrite(LEDpin,HIGH);
  delay(250);  
  digitalWrite(LEDpin,LOW);
  delay(250);  
  digitalWrite(LEDpin,HIGH);
  delay(250);  
  digitalWrite(LEDpin,LOW);
}

void PANICSTOP(){
//  stepper_X.stop();
//  stepper_Y.stop();
}


bool move2D(int newx,int newy, int onoff){
  //takes in stepper commands
  
  long positions[2]; // Array of desired stepper positions
  
  positions[0] = newx;
  positions[1] = newy;
  stepper2D.moveTo(positions); // set new position commands

  // Blocks until all are in position
  Serial.println("running");
  while( stepper2D.run() ){ 
    if(digitalRead(limPinX) == LOW){
      //stop X axis
      return false;
    }
    if(digitalRead(limPinY) == LOW){
      //stop Y axis
      return false;
    }
  }

  return true;
}

void loop() 
{
  cmd_x_mm = 100;
  cmd_y_mm = 100;
  cmd_onoff = 1;

  Serial.println(cmd_x_mm);
  Serial.println(cmd_y_mm);
  Serial.println(cmd_onoff);

  //convert to steps
  cmd_x_mm = cmd_x_mm * X_MM_2_STEPS;
  cmd_y_mm = cmd_y_mm * Y_MM_2_STEPS;

  //move it
  bool moved = move2D(cmd_x_mm,cmd_y_mm,cmd_onoff);
  delay(1000);
  
  cmd_x_mm = 0;
  cmd_y_mm = 0;
  cmd_onoff = 1;

  Serial.println(cmd_x_mm);
  Serial.println(cmd_y_mm);
  Serial.println(cmd_onoff);

  //convert to steps
  cmd_x_mm = cmd_x_mm * X_MM_2_STEPS;
  cmd_y_mm = cmd_y_mm * Y_MM_2_STEPS;

  //move it
  moved = move2D(cmd_x_mm,cmd_y_mm,cmd_onoff);
  delay(1000);
  
  if(digitalRead(startBTN) == LOW){
    //PAUSE = true;
  }
  else if(digitalRead(startBTN) == HIGH){
    //PAUSE = false;
  }
}


//Rods serial packets
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
