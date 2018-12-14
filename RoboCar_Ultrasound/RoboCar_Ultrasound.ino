// scanning mode

#include <AFMotor.h>
#include <SoftwareSerial.h>
#include <Servo.h>

#define echoPin A1 // Echo Pin
#define trigPin A0 // Trigger Pin
#define threshDist 1500
#define turnTime 1500
#define backTime 750
#define stopTime 500
#define speedAll 240
#define halfSpeed 150
#define buttonpin 9 //servo2
#define servopin1 10 //servo1


//motor objects
AF_DCMotor motor_lf(1);
AF_DCMotor motor_rf(4);
AF_DCMotor motor_lb(2);
AF_DCMotor motor_rb(3);

//button variables
int buttonVal = 0;

//variables for distance sensor
int maximumRange = 8000; // Maximum range needed
long getDist;
unsigned long timestart, timenow;
bool avoidance = false;
bool wemovin = false;
long mDist = threshDist;
long lDist = threshDist;
long rDist = threshDist;

//servo vars
Servo scanservo;
int servomid = 99;
int servoradius = 45;
int servomax = servomid + servoradius;
int servomin = servomid - servoradius;
int dstep = 2;
int servopos = servomid;
int sectionradius = 10;
int leftlimit = servomid + sectionradius;
int rightlimit = servomid - sectionradius;
int locflip = 0;


void setup() {
  Serial.begin(9600);           // set up Serial library at 9600 bps
  Serial.println("RoboCar!");

  //ultrasound pins (A0 and A1 on motor board)
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  //servo
  scanservo.attach(servopin1); 
  scanservo.write(servopos);
  delay(40);

  //control button
  pinMode(buttonpin,INPUT_PULLUP);
   
  // turn on motors
  motor_lf.setSpeed(0);
  motor_lf.run(RELEASE);
  motor_rf.setSpeed(0);
  motor_rf.run(RELEASE);
  motor_lb.setSpeed(0);
  motor_lb.run(RELEASE);
  motor_rb.setSpeed(0);
  motor_rb.run(RELEASE);
}


//Control all motor speeds
void setMotorSpeeds(int lf, int rf, int lb, int rb){
  //left front
  if(lf < 0){
    motor_lf.setSpeed(abs(lf));
    motor_lf.run(FORWARD);
  }
  else if(lf == 0){
    motor_lf.setSpeed(abs(lf));
    motor_lf.run(RELEASE);
  }
  else{
    motor_lf.setSpeed(abs(lf));
    motor_lf.run(BACKWARD);
  }
  //right front
  if(rf < 0){
    motor_rf.setSpeed(abs(rf));
    motor_rf.run(FORWARD);
  }
  else if(rf == 0){
    motor_rf.setSpeed(abs(rf));
    motor_rf.run(RELEASE);
  }
  else{
    motor_rf.setSpeed(abs(rf));
    motor_rf.run(BACKWARD);
  }
  //left back
  if(lb < 0){
    motor_lb.setSpeed(abs(lb));
    motor_lb.run(FORWARD);
  }
  else if(lb == 0){
    motor_lb.setSpeed(abs(lb));
    motor_lb.run(RELEASE);
  }
  else{
    motor_lb.setSpeed(abs(lb));
    motor_lb.run(BACKWARD);
  }
  //right back
  if(rb < 0){
    motor_rb.setSpeed(abs(rb));
    motor_rb.run(FORWARD);
  }
  else if(rb == 0){
    motor_rb.setSpeed(abs(rb));
    motor_rb.run(RELEASE);
  }
  else{
    motor_rb.setSpeed(abs(rb));
    motor_rb.run(BACKWARD);
  }
}



//get ultrasound distance
long getDistance(){
  long duration, distance; // Duration used to calculate distance
  
  //trigger off
  digitalWrite(trigPin, LOW); 
  delayMicroseconds(2); 
  
  //trigger on
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10); 

  //trigger off and read
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  
  //Calculate the distance (in cm) based on the speed of sound.
  distance = duration/58.2;

  //keep in a sensible range
  if(distance > maximumRange){
    distance = maximumRange;
  }
}

//control functions
void stopit(){
  setMotorSpeeds(0,0,0,0);//stop
}
void turnavoidL(){
  setMotorSpeeds(0,speedAll,0,speedAll); //left
}
void turnavoidR(){
  setMotorSpeeds(speedAll,0,speedAll,0);//right
}
void goforward(){
  setMotorSpeeds(speedAll,speedAll,speedAll,speedAll); //forward
}
void gobackward(){
  setMotorSpeeds(-speedAll,-speedAll,-speedAll,-speedAll); //backward
}


//scan direction
void updateScan(){
  //initialize values
  lDist = threshDist;
  mDist = threshDist;
  rDist = threshDist;

  //loop it
  while(1){
    //update servo scanner pos
    servopos = servopos + dstep;
  
    //keep in bounds
    if(servopos >= servomax){
      servopos = servomax;
      dstep = -dstep;
    }
    if(servopos <= servomin){
      servopos = servomin;
      dstep = -dstep;
      locflip = 1;
    }
  
    //write to that position
    scanservo.write(servopos); 
    delay(25);
  
    //check distances
    getDist = getDistance();
  
    //do moving averages
    if(servopos < rightlimit){
      rDist = (rDist + getDist) / 2;
    }
    else if(servopos >= rightlimit && servopos <= leftlimit){
      mDist = (mDist + getDist) / 2;
    }
    else if(servopos > leftlimit){
      lDist = (lDist + getDist) / 2;
    }

    //we done
    if(servopos > servomid && locflip == 1){
      break;
    }
  
  }

  locflip = 0;
  servopos = servomid;
  scanservo.write(servopos); 
  delay(15);
}


//loop forever
void loop() {
  
  //read the on/off button
  buttonVal = digitalRead(buttonpin);

  if(buttonVal == 1){
    //if we are okay goin
    
    //first loop just go
    if(!wemovin){
      wemovin = true;
      goforward();
    }

    //check distances
    getDist = getDistance();
  
    //see if we need to avoid or keep going
    if(getDist < threshDist && !avoidance){
      //turn if we about to hit something
      avoidance = true;
      Serial.println("We're Avoiding");
  
      //avoidance routine
      stopit();
      delay(stopTime);
      gobackward();
      delay(backTime);
      stopit();

      //updates servo and get distance
      updateScan();
      
      //check which way we should turn
      if(lDist >= rDist){
        turnavoidL();
      }
      else if(rDist > lDist){
        turnavoidR();
      }
      timestart = millis();
    }
    else if(avoidance){
      //check if we can go straight again yet
      timenow = millis();
      if(timenow - timestart > turnTime && getDist > threshDist){
        avoidance = false;
        Serial.println("Now We're Going");
        goforward();
      }
    }
    else{
      //can't think of anythign better to do
      //goforward();
    }
    
    
    //Delay 50ms before next reading.
    delay(50);

  }
  else{
    //if button is off
    if(wemovin){
      //stops the bot if we dont want movement
      stopit();
      wemovin = false;
    }
  }
    

}
