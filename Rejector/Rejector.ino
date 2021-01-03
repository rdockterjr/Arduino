#include <AFMotor.h>


//Inputs/Buttons
#define ConveyorRelay A0
#define ActuatorRelay A1 
#define ActuatorAdvanceButton A2
#define ActuatorRetractButton A3 

#define ActuatorStateAdvancing 0 
#define ActuatorStateAdvance 1
#define ActuatorStateRetracting 2
#define ActuatorStateRetract 3

#define ActuatorSpeed 75
#define ConveyorSpeed 255

int CurrentActuatorState = ActuatorStateRetract;

AF_DCMotor conveyor(1);
AF_DCMotor actuator(2);

void setup() {
  Serial.begin(9600);           // set up Serial library at 9600 bps
  Serial.println("Motor test!");

  // turn on conveyor
  conveyor.setSpeed(0);
  conveyor.run(RELEASE);

  // turn on actuator
  actuator.setSpeed(0);
  actuator.run(RELEASE);

  // Button Modes
  pinMode(ConveyorRelay, INPUT_PULLUP); 
  pinMode(ActuatorRelay, INPUT_PULLUP);
  pinMode(ActuatorAdvanceButton, INPUT_PULLUP);
  pinMode(ActuatorRetractButton, INPUT_PULLUP);
}

//Actuator State Machine
void ControlActuator(int input){
  int buttonval = 0;
  if(CurrentActuatorState == ActuatorStateRetract){
    //We are retracted, wait for command to advance
    if(input == 0){
      Serial.println("ActuatorStateAdvancing");
      CurrentActuatorState = ActuatorStateAdvancing;
    }
    else{
      actuator.run(BACKWARD);
      actuator.setSpeed(0);
    }
  }
  if(CurrentActuatorState == ActuatorStateAdvancing){
    //Advance until we hit button
    buttonval = digitalRead(ActuatorAdvanceButton);
    if(buttonval == 1){
      actuator.run(FORWARD);
      actuator.setSpeed(ActuatorSpeed);
    }
    else{
      actuator.run(FORWARD);
      actuator.setSpeed(0);
      CurrentActuatorState = ActuatorStateAdvance;
      Serial.println("ActuatorStateAdvance");
    }
  }
  if(CurrentActuatorState == ActuatorStateAdvance){
    //We are advanced, wait for command to go back
    if(input == 1){
      Serial.println("ActuatorStateRetracting");
      CurrentActuatorState = ActuatorStateRetracting;
    }
    else{
      actuator.run(FORWARD);
      actuator.setSpeed(0);
    }
  }
  if(CurrentActuatorState == ActuatorStateRetracting){
    //Retract until we hit the button
    buttonval = digitalRead(ActuatorRetractButton);
    if(buttonval == 1){
      actuator.run(BACKWARD);
      actuator.setSpeed(ActuatorSpeed);
    }
    else{
      actuator.run(BACKWARD);
      actuator.setSpeed(0);
      CurrentActuatorState = ActuatorStateRetract;
      Serial.println("ActuatorStateRetract");
    }
  }
}

//Conveyor State Machine
void ControlConveyor(int input){
  if(input == 1){
    //stop
    conveyor.run(FORWARD);
    conveyor.setSpeed(0);
  }
  if(input == 0){
    //run
    conveyor.run(FORWARD);
    conveyor.setSpeed(ConveyorSpeed);
  }
  
}

void loop() {
  //read values
  int ConveyorInput = digitalRead(ConveyorRelay);
  int ActuatorInput = digitalRead(ActuatorRelay);

  //control states
  ControlActuator(ActuatorInput);
  ControlConveyor(ConveyorInput);

  //0.05 second loop
  delay(50);
}
