int i =0;
int state = 0;
int initstate = 0;
int switchstate =0;
int ledpin = 2;
int inpin = 3;

int passarray[] = {0,0,0,0};
void setup()
{
  pinMode(ledpin,OUTPUT);
  pinMode(inpin,INPUT);
  Serial.begin(9600);
  establishContact();
}

void loop()
{ 
  i = i +1;
  state = digitalRead(inpin);
  while(state == initstate){
    state = digitalRead(inpin);
    digitalWrite(ledpin,HIGH);
  }
  Serial.println("switch to: ");
  Serial.println(state);
  Serial.println("loop number: ");
  Serial.println(i);
  
  while(state == switchstate){
    state = digitalRead(inpin);
    digitalWrite(ledpin,LOW);
  }
  Serial.println("switch to: ");
  Serial.println(state);
  Serial.println("loop number: ");
  Serial.println(i);
}



void establishContact() {
   if(Serial.available() <= 0) {
     initstate = digitalRead(inpin);
     Serial.println("The switch staerted out as: ");
     Serial.println(initstate);
     if(initstate == 1){
      switchstate = 0; 
     }
     if(initstate == 0){
      switchstate = 1; 
     }
   }
}

int isEven(int n){
  int response;
  response = (n/2)*2;
  if(response == n)
  {
    return 1;
  } 
  else
  {
    return 0; 
  }
}
