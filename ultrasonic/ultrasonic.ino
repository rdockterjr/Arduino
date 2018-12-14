
#define echoPin 9 // Echo Pin
#define trigPin 11 // Trigger Pin
#define speaker 8

int maximumRange = 200; // Maximum range needed
int minimumRange = 0; // Minimum range needed
long duration, distance; // Duration used to calculate distance

void setup() {
  Serial.begin (9600);
  pinMode(trigPin, OUTPUT);
  pinMode(speaker, OUTPUT);
  pinMode(echoPin, INPUT);
}

int tom = 0;

void loop() {
  digitalWrite(trigPin, LOW); 
  delayMicroseconds(2); 

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10); 
 
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
 
  //Calculate the distance (in cm) based on the speed of sound.
  distance = duration/58.2;
 
  Serial.println(distance);
  
  tom = distance*50;
  
  digitalWrite(speaker, HIGH);
  delayMicroseconds(tom / 2);
 
  digitalWrite(speaker, LOW);
  delayMicroseconds(tom/2);

  delay(20);
}
