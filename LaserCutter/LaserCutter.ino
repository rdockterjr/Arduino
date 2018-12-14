#include <AccelStepper.h>
#include <MultiStepper.h>
#include <Servo.h>

bool DEBUG = true;
bool PEN = false;
bool LASER = true;

//calibrated for 1/32 steps
//long X_MM_2_STEPS = 160; 
//long Y_MM_2_STEPS = 160;
long X_MM_2_STEPS = 40; 
long Y_MM_2_STEPS = 40;

#define DEFAULT_SPEED 10000
#define MAX_SPEED 1000

#define MAX_X_MM 400
#define MAX_Y_MM 330

//pins
#define LEDpin 3
#define startBTN A5
#define limPinX A4
#define limPinY A1


// Define a stepper and the pins it will use
//AccelStepper stepper_X(AccelStepper::DRIVER,3,2); //(DRIVER,Step,Dir)
//AccelStepper stepper_Y(AccelStepper::DRIVER,12,11); //(DRIVER,Step,Dir)
AccelStepper stepper_X(AccelStepper::DRIVER,8,11); //(DRIVER,Step,Dir)
AccelStepper stepper_Y(AccelStepper::DRIVER,12,13); //(DRIVER,Step,Dir)


//Do multi Stepper
MultiStepper stepper2D;


//Define servo/ laser
Servo PenServo;
int servopin = 6;
int SERVOUP = 90;
int SERVODOWN = 60;

int laserpin = 6;



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
long cmd_x_stp = 0;
long cmd_y_stp = 0;
long cmd_x_long = 0;
long cmd_y_long = 0;
int cmd_x_mm = 0;
int cmd_y_mm = 0;
int cmd_onoff = 0; // 0 off, 1 on

long prev_x = 0;
long prev_y = 0; 


//controls
bool PAUSE = false;
int PACKET = false;



void setup() 
{         
  Serial.begin(9600);
  
  //Setup Steppers (dont setSpeed(), thats managed by multistepper)
  stepper_X.setMaxSpeed(MAX_SPEED);
  stepper_Y.setMaxSpeed(MAX_SPEED);
  
  //set pin modes
  pinMode(LEDpin, OUTPUT); 
  pinMode(startBTN, INPUT_PULLUP); 
  pinMode(limPinX, INPUT_PULLUP); 
  pinMode(limPinY, INPUT_PULLUP); 

  if(PEN){
    //Servo
    PenServo.attach(servopin);
    PenServo.write(SERVOUP);
  }
  if(LASER){
    //Laser
    pinMode(laserpin,OUTPUT);
    digitalWrite(laserpin,LOW);
  }

  //flash LED
  for( int kk =0; kk < 4; kk++){
    digitalWrite(LEDpin,HIGH);
    delay(250);  
    digitalWrite(LEDpin,LOW);
    delay(250);  
  }

  Serial.println("homing sequence");
  //go through homing maneuver
  HomingSequence();

  // Then give them to MultiStepper to manage
  stepper2D.addStepper(stepper_X);//array[0]
  stepper2D.addStepper(stepper_Y);//array[1]
}

void PANICSTOP(){
//  stepper_X.stop();
//  stepper_Y.stop();
}


//will run stepper back until limit switch hit
void HomingSequence(){
  if(DEBUG){
    Serial.println("Started Homing");
  }

  if(PEN){
    PenServo.write(SERVOUP);
  }
  if(LASER){
    digitalWrite(laserpin,LOW);
  }

  long stepsize = -80000; //some ridiculously big motion

  stepper_X.setMaxSpeed(MAX_SPEED);
  stepper_Y.setMaxSpeed(MAX_SPEED);
  stepper_X.setAcceleration(10000);
  stepper_Y.setAcceleration(10000);

  //first do X axis
  stepper_X.move(stepsize);
  
  while( stepper_X.run() ){ 
    if(digitalRead(limPinX) == LOW){
      //zero x axis

      //stop it and reset zero
      stepper_X.stop();
      stepper_X.setCurrentPosition(0);

      if(DEBUG){
        Serial.print("zero x: ");
        Serial.println(stepper_X.currentPosition());
      }
      
      break;
    }
  }

  //now do Y axis
  stepper_Y.move(stepsize);
  
  while( stepper_Y.run() ){ 
    if(digitalRead(limPinY) == LOW){
      //zero y axis
      
      //stop it and reset zero
      stepper_Y.stop();
      stepper_Y.setCurrentPosition(0);

      if(DEBUG){
        Serial.print("zero y: ");
        Serial.println(stepper_Y.currentPosition());
      }

      //leave while loop
      break;
    }
  }

  if(DEBUG){
    Serial.println("Finished Homing");
  }

  stepper_X.setMaxSpeed(MAX_SPEED);
  stepper_Y.setMaxSpeed(MAX_SPEED);
}


//main move commands
bool move2D(long newx,long newy, int onoff){
  //takes in stepper commands

  if(onoff == 0){
    //Marker Up
    digitalWrite(LEDpin,HIGH);
    if(PEN){
      PenServo.write(SERVOUP);
    }
    if(LASER){
      digitalWrite(laserpin,LOW);
    }
  }
  if(onoff == 1){
    //Marker Down/ laser on
    digitalWrite(LEDpin,LOW);
    if(PEN){
      PenServo.write(SERVODOWN);
    }
    if(LASER){
      digitalWrite(laserpin,HIGH);
    }
  }
  
  
  long positions[2]; // Array of desired stepper positions
  
  positions[0] = newx;
  positions[1] = newy;
  stepper2D.moveTo(positions); // set new position commands

  // Blocks until all are in position
  if(DEBUG){
    Serial.println("running");
  }
  while( stepper2D.run() ){ 
    if(digitalRead(limPinX) == LOW){
      //maybe do something
    }
    if(digitalRead(limPinY) == LOW){
      //maybe do something
    }

    //E-STOP for safety and shit
    while(digitalRead(startBTN) == LOW){
        PAUSE = true;
    }
  }
  prev_x = newx;
  prev_y = newy;

  if(LASER){
    digitalWrite(laserpin,LOW);
  }
  return true;
}


//main loop
void loop() 
{
  //check if we're pausing
  if(!PAUSE){
    
    //check serial packet
    PACKET = receivePacket();
    if(PACKET == 1){
      
      //grab data from packet
      cmd_x_mm = newData[0];
      cmd_y_mm = newData[1];
      cmd_onoff = newData[2];

      //print them out
      if(DEBUG){
        Serial.println(cmd_x_mm);
        Serial.println(cmd_y_mm);
        Serial.println(cmd_onoff);
      }
  
      //limit X and Y
      if(cmd_x_mm > MAX_X_MM ){
        cmd_x_mm = MAX_X_MM;
      }
      if(cmd_y_mm > MAX_Y_MM ){
        cmd_y_mm = MAX_Y_MM;
      }
      if(cmd_x_mm < 0 ){
        cmd_x_mm = 0;
      }
      if(cmd_y_mm < 0 ){
        cmd_y_mm = 0;
      }

      //make em longs to avoid overflow
      cmd_x_long = (long) cmd_x_mm;
      cmd_y_long = (long) cmd_y_mm;

      //convert to steps
      cmd_x_stp = (long) (cmd_x_long * X_MM_2_STEPS);
      cmd_y_stp = (long) (cmd_y_long * Y_MM_2_STEPS);

      if(DEBUG){
        Serial.println(cmd_x_stp);
        Serial.println(cmd_y_stp);
      }

      //move it
      bool moved = move2D(cmd_x_stp,cmd_y_stp,cmd_onoff);

      //if it was succesful ask for next command
      if(moved){
        Serial.println("G");
      }
    }
  }
  
  if(digitalRead(startBTN) == LOW){
    PAUSE = true;

  }
  else if(digitalRead(startBTN) == HIGH){
    PAUSE = false;
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
