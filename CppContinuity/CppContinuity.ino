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
char Z[] = "Z";
void setup()
{
  pinMode(ledpin,OUTPUT);
  pinMode(Base1Pin,INPUT);
  pinMode(S1pin,INPUT);
  pinMode(Base2Pin,INPUT);
  pinMode(S2pin,INPUT);
  Serial.begin(9600);
  establishContact();
}

void loop()
{ 
  i = i +1;
  S1state = digitalRead(S1pin);
  Base1State = digitalRead(Base1Pin);
  S2state = digitalRead(S2pin);
  Base2State = digitalRead(Base2Pin);
  
  if(whichstand == 1){
    if(S2state == 0 || Base2State == 0){
      whichstand = 2; //check if ring has been transferred
    }
    if(Base1State == 1 && hasleft == 0){
      //ring hasn't been started on base yet
      delay(50);
    }
    else if(Base1State == 0 && hasleft == 0){
      printCombo(A, 0, 0); //ring has started on base
      S1state = digitalRead(S1pin);
      Base1State = digitalRead(Base1Pin);
      S2state = digitalRead(S2pin);
      Base2State = digitalRead(Base2Pin);
      while(Base1State == 0){
          S1state = digitalRead(S1pin);
          Base1State = digitalRead(Base1Pin);
          S2state = digitalRead(S2pin);
          Base2State = digitalRead(Base2Pin);
      }
      //ring leaves base
      starttime = millis();
      printCombo(A, starttime, 1); //combo A for base discontinuity
      hasleft = 1;
    }
    else if(Base1State == 0 && hasleft == 1){
      //ring drop back to base
      printCombo(A, 0, 2); //combo A for base continuity
      S1state = digitalRead(S1pin);
      Base1State = digitalRead(Base1Pin);
      S2state = digitalRead(S2pin);
      Base2State = digitalRead(Base2Pin);
      while(Base1State == 0){
          S1state = digitalRead(S1pin);
          Base1State = digitalRead(Base1Pin);
          S2state = digitalRead(S2pin);
          Base2State = digitalRead(Base2Pin);
      }
      delay(20);
    }
    else if(Base1State == 1){
      //now checking ring hits
      S1state = digitalRead(S1pin);
      Base1State = digitalRead(Base1Pin);
      S2state = digitalRead(S2pin);
      Base2State = digitalRead(Base2Pin);
      while(S1state == 1){
        if(S2state == 0 || Base2State == 0){
          whichstand = 2; //check if ring has been transferred
          break;
        }
        S1state = digitalRead(S1pin);
        Base1State = digitalRead(Base1Pin);
        S2state = digitalRead(S2pin);
        Base2State = digitalRead(Base2Pin);
        digitalWrite(ledpin,LOW);
      }
      printCombo(B, 0, 1); //combo B for ring continuity
      
      knockstarttime = millis();
      delay(10);
      S1state = digitalRead(S1pin);
      Base1State = digitalRead(Base1Pin);
      S2state = digitalRead(S2pin);
      Base2State = digitalRead(Base2Pin);
      while(S1state == 0 && whichstand == 1){
        if(S2state == 0 || Base2State == 0){
          whichstand = 2; //check if ring has been transferred
          break;
        }
        S1state = digitalRead(S1pin);
        Base1State = digitalRead(Base1Pin);
        S2state = digitalRead(S2pin);
        Base2State = digitalRead(Base2Pin);
        digitalWrite(ledpin,HIGH);
      }
      S1state = digitalRead(S1pin);
      Base1State = digitalRead(Base1Pin);
      S2state = digitalRead(S2pin);
      Base2State = digitalRead(Base2Pin);
      if(S2state == 0 || Base2State == 0){
          whichstand = 2; //check if ring has been transferred
      }
      if(whichstand == 1){
        knocktotaltime = millis() - knockstarttime;
        printCombo(B, knocktotaltime, 0); //combo B for ring continuity
      }
    }
    else{ 
      printCombo(Z, 0, 0);
    }
  }
  
  //ring has switched
  if(whichstand == 2){
    S1state = digitalRead(S1pin);
    Base1State = digitalRead(Base1Pin);
    S2state = digitalRead(S2pin);
    Base2State = digitalRead(Base2Pin);
    if(Base2State == 0){
      endtime = millis() - starttime;
      printCombo(D, endtime, 1); //ring has finished on second base
    }
    else if(Base2State == 1){
      //now checking ring hits
      S1state = digitalRead(S1pin);
      Base1State = digitalRead(Base1Pin);
      S2state = digitalRead(S2pin);
      Base2State = digitalRead(Base2Pin);
      while(S2state == 1){
        S1state = digitalRead(S1pin);
        Base1State = digitalRead(Base1Pin);
        S2state = digitalRead(S2pin);
        Base2State = digitalRead(Base2Pin);
        digitalWrite(ledpin,LOW);
        if(Base2State == 0){
          endtime = millis() - starttime;
          printCombo(D, endtime, 1); //ring has finished on second base
          whichstand = 3;
          break;
        }
      }
      printCombo(C, 0, 0); //combo C for ring continuity
      S1state = digitalRead(S1pin);
      Base1State = digitalRead(Base1Pin);
      S2state = digitalRead(S2pin);
      Base2State = digitalRead(Base2Pin);
      knockstarttime = millis();
      delay(10);
      while(S2state == 0 && whichstand == 2){
        S1state = digitalRead(S1pin);
        Base1State = digitalRead(Base1Pin);
        S2state = digitalRead(S2pin);
        Base2State = digitalRead(Base2Pin);
        if(Base2State == 0){
          endtime = millis() - starttime;
          printCombo(D, endtime, 1); //ring has finished on second base
          whichstand = 3;
          break;
        }
        digitalWrite(ledpin,HIGH);
      }
      if(whichstand == 3){
        endtime = millis() - starttime;
        printCombo(D, endtime, 1); //ring has finished on second base
      }
      if(whichstand == 3){
        knocktotaltime = millis() - knockstarttime;
        printCombo(C, knocktotaltime, 0); //combo B for ring continuity
      }
    }
    else{
     printCombo(Z, 0, 0);
    }
  }
  
  if(whichstand == 3){
  }

}



void establishContact() {
   if(Serial.available() <= 0) {
     initstate = digitalRead(S1pin);
     if(initstate == 0){
      switchstate = 1;
     } 
     if(initstate == 1){
      switchstate = 0;
     }
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
