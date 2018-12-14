#include <Servo.h> 

//moving average
float alpha = 0.4; // factor to tune
float avg = 180.0;


//Servos
int leftPin = 7;
int rightPin = 8;
Servo leftServo;
Servo rightServo;

//pots
int potPin = 0;
int potVal = 0;
float potLogType = 10; // wild guess base-10 logarithmic
int scale = 330; // experiment...
int linearized = 0;

//limits
int leftClose = 20;
int rightClose = 160;
int leftOpen = 180;
int rightOpen = 0;

//angles
int leftAngle = leftOpen;
int rightAngle = rightOpen;

void setup() 
{ 
  Serial.begin(9600);
  
  leftServo.attach(leftPin);
  rightServo.attach(rightPin);
  
  leftServo.write(leftOpen);
  rightServo.write(rightOpen);
} 

void loop() {
  delay(200);
  
  //read pot and convert to left and right grasper angles
  potVal = analogRead(potPin); //0 - 1023
  linearized = (int)((log((float)potVal) / log(potLogType)) * scale);
  leftAngle = map(linearized, 0, 995, 0, 179); 
  avg = alpha * float(leftAngle) + (1-alpha) * avg;
  leftAngle = int(avg);
  rightAngle = 180 - leftAngle;
  
  //bound angles
  if(leftAngle < leftClose){
    leftAngle = leftClose;
  }
  if(rightAngle > rightClose){
    rightAngle = rightClose;
  }

  Serial.print(rightAngle);
  Serial.print(',');
  Serial.println(leftAngle);
  
  //write servo angles
  leftServo.write(leftAngle); 
  rightServo.write(rightAngle); 
} 

