void setup(){
  pinMode(23,INPUT_PULLUP);
  pinMode(24,OUTPUT);
  Serial.begin(9600);
  
}

int SVAL = 0;
void loop(){
  SVAL = digitalRead(23);
  if(SVAL == 1){
   digitalWrite(24,HIGH); 
   Serial.println("Wire Tuch");
  }
  else{
   digitalWrite(24,LOW); 
  }
  
}
