//declarations
int i =0;
int S1state = 0;
int Base1State = 0;
int S2state = 0;
int Base2State = 0;
int hasleft = 0;

unsigned long starttime;
unsigned long endtime;
unsigned long knockstarttime;
unsigned long knocktotaltime;

int initstate = 0;
int switchstate =0;
int whichstand = 1;

int ledpin = 2;
int S1pin = 3;
int Base1Pin = 4;
int S2pin = 5;
int Base2Pin = 6;

char A[] = "A";
char B[] = "B";
char C[] = "C";
char D[] = "D";

int readpin = 2;
int writepin = 4;
void setup()
{
  pinMode(ledpin,OUTPUT);
//  pinMode(Base1Pin,INPUT);
//  pinMode(S1pin,INPUT);
//  pinMode(Base2Pin,INPUT);
//  pinMode(S2pin,INPUT);
pinMode(readpin,INPUT_PULLUP);
pinMode(writepin,OUTPUT);
  Serial.begin(9600);
}

//void loop()
//{ 
//  i = i +1;
//  S1state = digitalRead(S1pin);
//  Base1State = digitalRead(Base1Pin);
//  S2state = digitalRead(S2pin);
//  Base2State = digitalRead(Base2Pin);
//
//  while(S1state == 1 && S2state == 1 && Base1State == 1 && Base2State == 1){
//    S1state = digitalRead(S1pin);
//    Base1State = digitalRead(Base1Pin);
//    S2state = digitalRead(S2pin);
//    Base2State = digitalRead(Base2Pin);
//    digitalWrite(ledpin,LOW);
//  }
//  if(S1state == 0){
//    printCombo(B, knocktotaltime, 0);
//  }
//  if(Base1State == 0){
//    printCombo(A, knocktotaltime, 0);
//  }
//  if(S2state == 0){
//    printCombo(C, knocktotaltime, 0);
//  }
//  if(Base2State == 0){
//    printCombo(D, knocktotaltime, 0);
//  }
//  while(S1state == 0 || S2state == 0 || Base1State == 0 || Base2State == 0){
//    S1state = digitalRead(S1pin);
//    Base1State = digitalRead(Base1Pin);
//    S2state = digitalRead(S2pin);
//    Base2State = digitalRead(Base2Pin);
//    digitalWrite(ledpin,HIGH);
//  }
//
//}

void loop(){
  int read2 = 0;
 read2 = digitalRead(readpin);
   if(read2 == 1){
    digitalWrite(writepin,HIGH); 
   }
   else{
    digitalWrite(writepin, LOW); 
   }
}


void printCombo(char* code, unsigned long time, int updown)
{

  Serial.print('H'); // unique header to identify start of message
  Serial.print(",");
  Serial.print(code);
  Serial.print(",");
  Serial.print(time,DEC);
  Serial.print(",");
  Serial.print(updown,DEC);
  Serial.print(",");
  Serial.print("Q");  // note Q is sent to mark end of line
  Serial.println();  // send a cr/lf
}
