int analogPin = 6; 
int val = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {

   val = analogRead(analogPin);    // read the input pin

  Serial.println(val);             // debug value

  delay(10);
}
