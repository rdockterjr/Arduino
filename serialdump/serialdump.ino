int val = 0;
int potval = 0;
float percent = 90.0;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available() > 0) {
    // get incoming byte:
    int inByte = Serial.parseInt();
  }
  
  potval = analogRead(0);
  Serial.print(val);
  Serial.print(",");
  Serial.print(potval);
  Serial.print(",");
  Serial.println(percent);
  
  val = val + 1;
  delay(500);
}
