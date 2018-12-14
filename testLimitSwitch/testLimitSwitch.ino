int X_MIN_PIN = A4;
int X_MAX_PIN = A5;

int Y_MIN_PIN = A1;
int Y_MAX_PIN = A0;

void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once:
  pinMode(X_MIN_PIN, INPUT_PULLUP);
  pinMode(X_MAX_PIN, INPUT_PULLUP);

  pinMode(Y_MIN_PIN, INPUT_PULLUP);
  pinMode(Y_MAX_PIN, INPUT_PULLUP);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.print(digitalRead(X_MIN_PIN));
  Serial.print(digitalRead(X_MAX_PIN));
  Serial.print(digitalRead(Y_MIN_PIN));
  Serial.println(digitalRead(Y_MAX_PIN));

  delay(200);
}
