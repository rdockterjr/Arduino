long randNumber1;
long randNumber2;

byte byteRead;
int bluetoothOn = 0;

void setup(){
  Serial.begin(9600);
}

void loop() {
  if(bluetoothOn == 1){
    // print a random number from 0 to 9
    randNumber1 = random(10);
    randNumber2 = random(10);
    
    Serial.print(10); 
    Serial.print(",");
    Serial.print(randNumber1);
    Serial.print(",");
    Serial.print(randNumber2);
    Serial.print(",");
    Serial.print(100);
    Serial.print(",");
    Serial.print(50);
    Serial.print(",");
    Serial.println(1);
  
  
    delay(100);
  }
  else{
    if (Serial.available()) {
      /* read the most recent byte */
      byteRead = Serial.read();
      
      if(byteRead == 83 || byteRead == 115){
        bluetoothOn = 1;
        delay(100);
      }
    }
  }
  
}
