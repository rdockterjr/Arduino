int led = 7; // Pin 13
int btn = 8;

int btnValInit = 0;
int btnVal = 0;
int on = 0;

void setup()
{
  pinMode(led, OUTPUT); // Set pin 7 as digital out
  pinMode(btn, INPUT_PULLUP); // Set pin 8 as input

  btnValInit = digitalRead(btn); 

  // Start up serial connection
  Serial.begin(9600); // baud rate
  Serial.flush();
}

void loop()
{
    byte byteread;
    // Read any serial input
    if (Serial.available() > 0)
    {
        byteread = Serial.read(); // Read in one char at a time
        if(byteread == 83 || byteread == 115){
          //if 'S' is sent
          digitalWrite(led, HIGH); // on
          delay(200);
          digitalWrite(led, LOW); // on
        }
    }


  btnVal = digitalRead(btn); 

  if (btnVal != btnValInit && on == 0)
  {
    on = 1;
    digitalWrite(led, HIGH); // on
    sendVals(24,32,36,true,false,50);
    delay(100);
  }
  else if (btnVal == btnValInit && on == 1)
  {
    on = 0;
    digitalWrite(led, LOW); // off
    delay(100);
  }


}


void sendVals(float val1, float val2, float val3, boolean val4, boolean val5, float val6){
  Serial.print(val1);
  Serial.print(",");
  Serial.print(val2);
  Serial.print(",");
  Serial.print(val3);
  Serial.print(",");
  if(val4){
    Serial.print("true");
  }
  else{
    Serial.print("false");
  }
  Serial.print(",");
  if(val5){
    Serial.print("true");
  }
  else{
    Serial.print("false");
  }
  Serial.print(",");
  Serial.println(val6);

}

