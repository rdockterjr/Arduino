int cnt=0;

void setup() {
  pinMode(6,INPUT);
  pinMode(3,OUTPUT);
  pinMode(2,OUTPUT);
  pinMode(4,OUTPUT);
  pinMode(5,OUTPUT);
  digitalWrite(4,HIGH);
  delay(1000);
  digitalWrite(4,LOW);
}

void loop() {
  
  if(digitalRead(6)==HIGH)
  {
  
      if(cnt<=20)
      {
        digitalWrite(2,HIGH);
        digitalWrite(3,HIGH);
        delay(1000);
        digitalWrite(2,LOW);
        digitalWrite(3,LOW);
        delay(1000);
      }
      else
      {
        digitalWrite(2,LOW);
        digitalWrite(3,LOW);
     
      }
      cnt++;
  }
  else
  {
    tone(5,440,500);
    delay(600);
  }


}
