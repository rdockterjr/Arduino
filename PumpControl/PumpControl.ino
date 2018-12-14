
void setup(){
 Serial.begin(9600); 
  pinMode(3,INPUT_PULLUP);
  pinMode(12,OUTPUT);
  pinMode(11,OUTPUT);
  digitalWrite(11,HIGH);
}
int returnbutton;
void loop(){
  returnbutton = digitalRead(3);
  if(returnbutton == 0){
   digitalWrite(12,HIGH); 
  }
  else{
   digitalWrite(12,LOW); 
  }
  delay(200);
}
