#define buttonpin 9 //servo2
//#define buttonpin 10 //servo1

void setup() {
  // put your setup code here, to run once:
pinMode(buttonpin,INPUT_PULLUP);
Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  int sensorValue = digitalRead(buttonpin);
  Serial.println(sensorValue);
}
