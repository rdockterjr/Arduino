

#include <Servo.h>


Servo myservo;  // create servo object to control a servo

//position variables
int csflag = 0;
int openservo = 100;
int closeservo = 1;
int defaultservo = openservo;
int servopin = 3;

//servo function
void initializeServo(){
  myservo.attach(servopin);  // attaches the servo on pin 9 to the servo object
  myservo.write(defaultservo); 
  csflag = 1;
}


//servo box control
void boxControl(bool open){
  if(open){
    if(csflag == 0){
      myservo.write(openservo);  
      csflag = 1;
    }
  }
  if(!open){
    if(csflag == 1){
      myservo.write(closeservo);  
      csflag = 0;
    }
  }
}


////////////////////Copy to Here




int incomingByte = 0;

void setup() {
  Serial.begin(9600);
  initializeServo();
}


//just dummy code
void loop() {
  if (Serial.available() > 0) {
    // read the incoming byte:
    incomingByte = Serial.parseInt();
    
    // say what you got:
    Serial.print("I received: ");
    Serial.println(incomingByte);

    if(incomingByte == 2){
      boxControl(false);
    }
    if(incomingByte == 3){
      boxControl(true);
    }
  }

  
}



