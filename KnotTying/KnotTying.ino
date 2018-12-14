int analogPin = 0;     // potentiometer wiper (middle terminal) connected to analog pin 3
int ledpin = 3;
int val = 0;
int valdiv = 0;
int division = 0;

int inbyte;
unsigned long serialdata;
int WHICH_TASK = 0;

void setup()
{
  Serial.begin(9600);
  //division = 1023/5;
}

void loop()
{
  //val = analogRead(analogPin);    // read the input pin
  //valdiv = val/division;
  getSerial();
  WHICH_TASK = serialdata;
  if(WHICH_TASK == 1){
   Serial.println("Well were doin tha first one "); 
  }
  else{
   Serial.println("the other one "); 
  }
  delay(1000);


}

long getSerial(){
  serialdata = 0;
  while(inbyte != '/'){
    inbyte = Serial.read();
    if(inbyte > 0 && inbyte != '/'){
      serialdata = serialdata*10 + inbyte - '0';
    } 
  }
  inbyte = 0;
  return serialdata;
}

