#define echoPin A1 // Echo Pin
#define trigPin A0 // Trigger Pin

int maximumRange = 200; // Maximum range needed
int minimumRange = 0; // Minimum range needed
long getDist;

void setup() {
  Serial.begin (9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

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
}

void loop() {
  getDist = getDistance();
  Serial.println(getDist);
  
  //Delay 50ms before next reading.
  delay(50);
}
