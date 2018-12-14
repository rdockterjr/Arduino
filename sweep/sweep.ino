#include <Servo.h>

#define echoPin A1 // Echo Pin
#define trigPin A0 // Trigger Pin

//variables for distance sensor
int maximumRange = 10000; // Maximum range needed
long getDist;

Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

//servo vars
int servomid = 99;
int servoradius = 45;
int servomax = servomid + servoradius;
int servomin = servomid - servoradius;
int dstep = 1;
int servopos = servomid;


void setup() {
  Serial.begin(9600);
  myservo.attach(10);  // attaches the servo on pin 9 to the servo object
  
  myservo.write(servopos); 

  //ultrasound pins (A0 and A1 on motor board)
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
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

void loop() {
  //update servo scanner
  servopos = servopos + dstep;
  if(servopos >= servomax){
    servopos = servomax;
    dstep = -dstep;
  }
  if(servopos <= servomin){
    servopos = servomin;
    dstep = -dstep;
  }

  myservo.write(servopos); 
  delay(15);
  
  //check distances
  getDist = getDistance();

  Serial.print(servopos);
  Serial.print(",");
  Serial.println(getDist);

  //Delay 50ms before next reading.
  delay(50);
}



