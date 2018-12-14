int onpin = 2;
int dirpin = 7;

void setup() {
  pinMode(onpin,OUTPUT);
  pinMode(dirpin,OUTPUT);
  digitalWrite(dirpin,LOW);
Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(onpin,LOW);
  delay(2);
  digitalWrite(dirpin,HIGH);
  delay(2);
  digitalWrite(onpin,HIGH);
  delay(2000);
  
  digitalWrite(onpin,LOW);
  delay(2);
  digitalWrite(dirpin,LOW);
  delay(2);
  digitalWrite(onpin,HIGH);
  delay(2000);
  
  digitalWrite(onpin,LOW);
  delay(2000);
  Serial.println("Rebecca Black");
}
