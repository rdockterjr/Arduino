#include <Servo.h>

//Define pins for the sensor
#define trigPin 6
#define echoPin 5

Servo servo1;

int pos1 = 0;
int cpos = 90;

int minp = 45;
int maxp = 135;

void setup() {

  servo1.attach(3);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  Serial.begin(9600);
  Serial.println("Ready");
  servo1.write(cpos);
}

void loop() {
  
  
  for(pos1 = cpos; pos1 < maxp; pos1++){
    servo1.write(pos1);
    long dist1 = getDist();
    long dist2 = getDist();
    long dist3 = getDist();
    long dist4 = getDist();
    long distance = (dist1 + dist2 + dist3 + dist4) / 4;
    Serial.print("Angle= ");  
    Serial.print(pos1);
    Serial.print(", Dist=");
    Serial.println(distance);
    cpos = pos1;
    delay(300);
  }
  delay(500);
  for(pos1 = cpos; pos1  > minp; pos1--){
    servo1.write(pos1);
    long dist1 = getDist();
    long dist2 = getDist();
    long dist3 = getDist();
    long dist4 = getDist();
    long distance = (dist1 + dist2 + dist3 + dist4) / 4;
    Serial.print("Angle= ");  
    Serial.print(pos1);
    Serial.print(", Dist=");
    Serial.println(distance);
    cpos = pos1;
    delay(300);
  }
   
}

long getDist(){
  long duration, distance;
  digitalWrite(trigPin, LOW);  // Added this line
  delayMicroseconds(2); // Added this line
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10); // Added this line
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration/2) / 29.1; 
  
  return distance;
}
