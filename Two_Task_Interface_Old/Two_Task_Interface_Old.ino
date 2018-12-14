//declare pins
int RING_1_RBC = 2; //ring-base
int RING_1_SC = 3; //S-continuty
int RING_1_LED = 4; // LED
int RING_1_BC = 5; // Base continuity
int RING_2_RBC = 6; //ring-base
int RING_2_SC = 7; // S continuity
int RING_2_LED = 8; // led
int RING_2_BC = 9; // base continuity
int KNOT_BUTTON = 10; //switch for done with knot
int KNOT_LED = 11; //led on pot
int KNOT_MOTOR = 12; // servo control
int KNOT_EYELET = 13; // Eyelet continuity
int KNOT_POT = 0; //analog pin for pot
int KNOT_CURRENT = 1; //analog pin monitor motor voltage

//declare pin read values
int RING_1_RBC_VAL = 0; //ring-base
int RING_1_SC_VAL = 0; //S-continuty
int RING_1_BC_VAL = 0; // Base continuity
int RING_2_RBC_VAL = 0; //ring-base
int RING_2_SC_VAL = 0; // S continuity
int RING_2_BC_VAL = 0; // base continuity
int KNOT_BUTTON_VAL = 0; //switch for done with knot
int KNOT_EYELET_VAL = 0; // Eyelet continuity
int KNOT_POT_VAL = 0; //analog pin for pot
int KNOT_CURRENT_VAL = 0; //analog pin monitor motor voltage

//Initial Pins
int RING_1_RBC_INIT = 0; //ring-base
int RING_1_SC_INIT = 0; //S-continuty
int RING_1_BC_INIT = 0; // Base continuity
int RING_2_RBC_INIT = 0; //ring-base
int RING_2_SC_INIT = 0; // S continuity
int RING_2_BC_INIT = 0; // base continuity
int KNOT_BUTTON_INIT = 0; //switch for done with knot
int KNOT_EYELET_INIT = 0; // Eyelet continuity
int KNOT_POT_INIT = 0; //analog pin for pot
int KNOT_CURRENT_INIT = 0; //analog pin monitor motor voltage

//declare states
int WHICH_TASK = 0;
int TOWER_STATE = 0;
int SWITCH_STATE = 0;
int TOWER_BREAK = 0;
int KNOT_SNAP = 0;
int Serial_Return_Val = 0;
boolean Serial_String_Complete = false;
int initError = 0;
int inbyte;
unsigned long serialdata;

//timing variables
int Tower_Start = 0;
int Tower_End = 0;
int Tower_Hit_Start = 0;
int Tower_Hit_End = 0;
int Knot_Start = 0;
int Knot_End = 0;
int Total_Tower_Touch = 0;

//Characters
//H and Q are protected
char A[] = "A";//Ring on base 1
char B[] = "B";//Ring on S 1
char C[] = "C";//Ring on S 2
char D[] = "D";//Ring on base 2
char E[] = "E";//Succesful Ring Tower Finish
char F[] = "F";//knot tying not started
char G[] = "G";//Knot tying in progress
char I[] = "I";//Knot tying done
char J[] = "J";//Knot tying motor running
char K[] = "K";//Knot tying motor finished
char V[] = "V";//GUI signaled change task mid task
char Y[] = "Y";//Auto Fail for ring tower
char Z[] = "Z";//Auto Fail for knot

void setup()
{
  //setup pins
  pinMode(RING_1_RBC,INPUT);
  pinMode(RING_1_SC,INPUT);
  pinMode(RING_1_LED,OUTPUT);
  pinMode(RING_1_BC,INPUT);
  pinMode(RING_2_RBC,INPUT);
  pinMode(RING_2_SC,INPUT);
  pinMode(RING_2_LED,OUTPUT);
  pinMode(RING_2_BC,INPUT);
  pinMode(KNOT_BUTTON,INPUT);
  pinMode(KNOT_EYELET,INPUT);
  pinMode(KNOT_LED,OUTPUT);
  pinMode(KNOT_MOTOR,OUTPUT);
  //KNOT_POT is analog (default read)
  establishContact();
  if(initError == 0){
    Serial.begin(9600);
  }
}

//TASK NUMBERS
//TASK 1: Ring tower transfer
//TASK 2: Knot Tying
//....

void loop()
{

  serialReader();
  if (Serial_String_Complete || SWITCH_STATE == 1){
    SWITCH_STATE = 1;
    if(Serial_Return_Val == 1){
      //Task 1
      WHICH_TASK == 1;
      int ret1;
      ret1 = performTask1();
       if(ret2 == 2){
          printcombo(V,1,1,1);
        }
        if(ret2 == 3){
          printcombo(Y,0,0,0);
        } 
    }
    else if(Serial_Return_Val == 2){
      //Task 1
      WHICH_TASK == 2;
      int ret2;
      ret2 = performTask2();
      if(ret2 == 2){
        printcombo(V,1,1,1);
      }
      if(ret2 == 3){
        printcombo(Z,0,0,0);
      }
    }
    SWITCH_STATE = 0;
  }
  delay(100); 
}


void establishContact() {
   if(Serial.available() <= 0) {
     RING_1_RBC_INIT = digitalRead(RING_1_RBC);
     RING_1_SC_INIT = digitalRead(RING_1_S);
     RING_1_BC_INIT = digitalRead(RING_1_BC);
     RING_2_RBC_INIT = digitalRead(RING_2_RBC);
     RING_2_SC_INIT = digitalRead(RING_2_SC);
     RING_2_BC_INIT = digitalRead(RING_2_BC);
     KNOT_BUTTON_INIT = digitalRead(KNOT_BUTTON);
     KNOT_EYELET_INIT = digitalRead(KNOT_EYELET);
     KNOT_POT_INIT = analogRead(KNOT_POT);
     KNOT_CURRENT_INIT = analogRead(KNOT_CURRENT);
     
     if(RING_1_RBC_INIT == 0)){
       initError = 1;
     } 
     if(RING_1_SC_INIT == 0)){
       initError = 1;
     }
     if(RING_1_BC_INIT == 1)){
       initError = 1;
     }
     if(RING_2_RBC_INIT == 0)){
       initError = 1;
     }
     if(RING_2_SC_INIT == 0)){
       initError = 1;
     }
     if(RING_2_BC_INIT == 1)){
       initError = 1;
     }
     if(KNOT_BUTTON_INIT == 1)){
       initError = 1;
     }
     if(KNOT_EYELET_INIT == 0)){
       initError = 1;
     }
   }
}

int performTask1(){
  TOWER_BREAK = 0;
  TOWER_STATE = 1;
  SWITCH_STATE = 0;
  
  i = i +1;
  RING_1_RBC_VAL = digitalRead(RING_1_RBC);
  RING_1_SC_VAL = digitalRead(RING_1_S);
  RING_1_BC_VAL = digitalRead(RING_1_BC);
  RING_2_RBC_VAL = digitalRead(RING_2_RBC);
  RING_2_SC_VAL = digitalRead(RING_2_SC);
  RING_2_BC_VAL = digitalRead(RING_2_BC);
  
  while(true){
    RING_1_RBC_VAL = digitalRead(RING_1_RBC);
    RING_1_SC_VAL = digitalRead(RING_1_S);
    RING_1_BC_VAL = digitalRead(RING_1_BC);
    RING_2_RBC_VAL = digitalRead(RING_2_RBC);
    RING_2_SC_VAL = digitalRead(RING_2_SC);
    RING_2_BC_VAL = digitalRead(RING_2_BC);
    serialReader();
    if (Serial_String_Complete){
      SWITCH_STATE = 1;
      return 2;
      break;
    }
    if(TOWER_STATE == 1 && SWITCH_STATE == 0 && TOWER_BREAK == 0){
      if(RING_2_SC_VAL == 0 || RING_2_RBC_VAL == 0){
        TOWER_STATE = 2; //check if ring has been transferred
      }
      if(RING_2_BC_VAL == 1 || RING_1_BC_VAL == 1){
        //Tower Break off - Fail
        TOWER_BREAK = 1;
        return 3;
        break;
      }
      serialReader();
      if (Serial_String_Complete){
        SWITCH_STATE = 1;
        return 2;
        break;
      }
      if(RING_1_RBC_VAL == 1 && hasleft == 0 && SWITCH_STATE == 0){
        //ring hasn't been started on base yet
        delay(50);
      }
      else if(RING_1_RBC_VAL == 0 && hasleft == 0 && SWITCH_STATE == 0 && TOWER_BREAK == 0){
        printCombo(A, 0, 0, 0); //ring has started on base
        RING_1_RBC_VAL = digitalRead(RING_1_RBC);
        RING_1_SC_VAL = digitalRead(RING_1_S);
        RING_1_BC_VAL = digitalRead(RING_1_BC);
        RING_2_RBC_VAL = digitalRead(RING_2_RBC);
        RING_2_SC_VAL = digitalRead(RING_2_SC);
        RING_2_BC_VAL = digitalRead(RING_2_BC);
        while(Base1State == 0){
            RING_1_RBC_VAL = digitalRead(RING_1_RBC);
            RING_1_SC_VAL = digitalRead(RING_1_S);
            RING_1_BC_VAL = digitalRead(RING_1_BC);
            RING_2_RBC_VAL = digitalRead(RING_2_RBC);
            RING_2_SC_VAL = digitalRead(RING_2_SC);
            RING_2_BC_VAL = digitalRead(RING_2_BC);
            serialReader();
            if (Serial_String_Complete){
              SWITCH_STATE = 1;
              return 2;
              break;
            }
            if(RING_2_BC_VAL == 1 || RING_1_BC_VAL == 1){
              //Tower Break off - Fail
              TOWER_BREAK = 1;
              return 3;
              break;
            }
        }
        //ring leaves base
        Tower_Start = millis();
        printCombo(A, Tower_Start, 1, 0); //combo A for base discontinuity
        hasleft = 1;
      }
      else if(RING_1_RBC_VAL == 1 && SWITCH_STATE == 0 && TOWER_BREAK == 0){
        //now checking ring hits
        RING_1_RBC_VAL = digitalRead(RING_1_RBC);
        RING_1_SC_VAL = digitalRead(RING_1_S);
        RING_1_BC_VAL = digitalRead(RING_1_BC);
        RING_2_RBC_VAL = digitalRead(RING_2_RBC);
        RING_2_SC_VAL = digitalRead(RING_2_SC);
        RING_2_BC_VAL = digitalRead(RING_2_BC);
        while(RING_1_SC_VAL == 1){
          //While No Touch
          if(RING_2_SC_VAL == 0 || RING_2_RBC_VAL == 0){
            TOWER_STATE = 2; //check if ring has been transferred
            break;
          }
          if(RING_2_BC_VAL == 1 || RING_1_BC_VAL == 1){
            //Tower Break off - Fail
            TOWER_BREAK = 1;
            return 3;
            break;
          }
          serialReader();
          if (Serial_String_Complete){
            SWITCH_STATE = 1;
            return 2;
            break;
          }
          RING_1_RBC_VAL = digitalRead(RING_1_RBC);
          RING_1_SC_VAL = digitalRead(RING_1_S);
          RING_1_BC_VAL = digitalRead(RING_1_BC);
          RING_2_RBC_VAL = digitalRead(RING_2_RBC);
          RING_2_SC_VAL = digitalRead(RING_2_SC);
          RING_2_BC_VAL = digitalRead(RING_2_BC);
          digitalWrite(RING_1_LED,LOW);
        }
        if(SWITCHSTATE == 0 && TOWER_BREAK == 0 && TOWER_STATE == 1){
          //Make sure that the break from continuity was not for other reasons
          printCombo(B, 0, 1, 0); //combo B for ring continuity
          Total_Tower_Touch += 1;
          Tower_Hit_Start = millis();
          delay(10);
          RING_1_RBC_VAL = digitalRead(RING_1_RBC);
          RING_1_SC_VAL = digitalRead(RING_1_S);
          RING_1_BC_VAL = digitalRead(RING_1_BC);
          RING_2_RBC_VAL = digitalRead(RING_2_RBC);
          RING_2_SC_VAL = digitalRead(RING_2_SC);
          RING_2_BC_VAL = digitalRead(RING_2_BC);
          while(RING_1_SC_VAL == 0 && TOWER_STATE == 1){
            //While touching
            RING_1_RBC_VAL = digitalRead(RING_1_RBC);
            RING_1_SC_VAL = digitalRead(RING_1_S);
            RING_1_BC_VAL = digitalRead(RING_1_BC);
            RING_2_RBC_VAL = digitalRead(RING_2_RBC);
            RING_2_SC_VAL = digitalRead(RING_2_SC);
            RING_2_BC_VAL = digitalRead(RING_2_BC);
            if(RING_2_SC_VAL == 0 || RING_2_RBC_VAL == 0){
              TOWER_STATE = 2; //check if ring has been transferred
              break;
            }
            if(RING_2_BC_VAL == 1 || RING_1_BC_VAL == 1){
              //Tower Break off - Fail
              TOWER_BREAK = 1;
              return 3;
              break;
            }
            serialReader();
            if (Serial_String_Complete){
              SWITCH_STATE = 1;
              return 2;
              break;
            }
            digitalWrite(RING_1_LED,HIGH);
          }
          if(SWITCHSTATE == 0 && TOWER_BREAK == 0 && TOWER_STATE == 1){
            Tower_Hit_End = millis() - Tower_Hit_Start;
            printCombo(B, Tower_Hit_End, 0, 0); //combo B for ring continuity
          }
          if(SWITCHSTATE == 1 || TOWER_BREAK == 1 || TOWER_STATE == 0){
            break;
          }
        }
      }
    }
    
    ///////////////////////RING SWITCH////////////////////////////
    if(TOWER_STATE == 2 && SWITCHSTATE == 0 && TOWER_BREAK == 0){
      RING_1_RBC_VAL = digitalRead(RING_1_RBC);
      RING_1_SC_VAL = digitalRead(RING_1_S);
      RING_1_BC_VAL = digitalRead(RING_1_BC);
      RING_2_RBC_VAL = digitalRead(RING_2_RBC);
      RING_2_SC_VAL = digitalRead(RING_2_SC);
      RING_2_BC_VAL = digitalRead(RING_2_BC);
      if(RING_2_BC_VAL == 1 || RING_1_BC_VAL == 1){
        //Tower Break off - Fail
        TOWER_BREAK = 1;
        return 3;
        break;
      }
      serialReader();
      if (Serial_String_Complete){
        SWITCH_STATE = 1;
        return 2;
        break;
      }
      if(RING_2_RBC_VAL == 0){
        TOWER_STATE = 3;
      }
      else if(RING_2_RBC_VAL == 1 && SWITCH_STATE == 0){
        //now checking ring hits
        RING_1_RBC_VAL = digitalRead(RING_1_RBC);
        RING_1_SC_VAL = digitalRead(RING_1_S);
        RING_1_BC_VAL = digitalRead(RING_1_BC);
        RING_2_RBC_VAL = digitalRead(RING_2_RBC);
        RING_2_SC_VAL = digitalRead(RING_2_SC);
        RING_2_BC_VAL = digitalRead(RING_2_BC);
        while(RING_2_SC_VAL == 1){
          //Not Touching
          RING_1_RBC_VAL = digitalRead(RING_1_RBC);
          RING_1_SC_VAL = digitalRead(RING_1_S);
          RING_1_BC_VAL = digitalRead(RING_1_BC);
          RING_2_RBC_VAL = digitalRead(RING_2_RBC);
          RING_2_SC_VAL = digitalRead(RING_2_SC);
          RING_2_BC_VAL = digitalRead(RING_2_BC);
          if(RING_2_BC_VAL == 1 || RING_1_BC_VAL == 1){
            //Tower Break off - Fail
            TOWER_BREAK = 1;
            return 3;
            break;
          }
          serialReader();
          if (Serial_String_Complete){
            SWITCH_STATE = 1;
            return 2;
            break;
          }
          if(RING_2_RBC_VAL == 0){
            TOWER_STATE = 3;
            break;
          }
          digitalWrite(RING_2_LED,LOW);
          
        }
        if(SWITCHSTATE == 0 && TOWER_BREAK == 0 && TOWER_STATE == 2){
          //Make sure that the break from continuity was not for other reasons
          printCombo(C, 0, 0, 1); //combo C for ring continuity
          RING_1_RBC_VAL = digitalRead(RING_1_RBC);
          RING_1_SC_VAL = digitalRead(RING_1_S);
          RING_1_BC_VAL = digitalRead(RING_1_BC);
          RING_2_RBC_VAL = digitalRead(RING_2_RBC);
          RING_2_SC_VAL = digitalRead(RING_2_SC);
          RING_2_BC_VAL = digitalRead(RING_2_BC);
          
          Total_Tower_Touch += 1;
          Tower_Hit_Start = millis();
          delay(10);
          while(RING_2_SC_VAL == 0 && TOWER_STATE == 2){
            RING_1_RBC_VAL = digitalRead(RING_1_RBC);
            RING_1_SC_VAL = digitalRead(RING_1_S);
            RING_1_BC_VAL = digitalRead(RING_1_BC);
            RING_2_RBC_VAL = digitalRead(RING_2_RBC);
            RING_2_SC_VAL = digitalRead(RING_2_SC);
            RING_2_BC_VAL = digitalRead(RING_2_BC);
            if(RING_2_RBC_VAL == 0){
              TOWER_STATE = 3;
              break;
            }
            if(RING_2_BC_VAL == 1 || RING_1_BC_VAL == 1){
              //Tower Break off - Fail
              TOWER_BREAK = 1;
              return 3;
              break;
            }
            serialReader();
            if (Serial_String_Complete){
              SWITCH_STATE = 1;
              return 2;
              break;
            }
            digitalWrite(RING_2_LED,HIGH);
          }
          if(TOWER_STATE == 2 && SWITCH_STATE == 0 && TOWER_BREAK == 0){
            Tower_Hit_End = millis() - Tower_Hit_Start;
            printCombo(D, Tower_Hit_End, 0, 0); //ring has finished on second base
          }
          if(TOWER_STATE == 3 || SWITCH_STATE == 1 || TOWER_BREAK == 1){
            break;
          }
        }
      }
    }
    if(SWITCHSTATE == 1 || TOWER_BREAK == 1 || TOWER_STATE == 3){
      break;
    }
  }
  
  if(SWITCH_STATE == 1){
   return 2; 
  }
  else if(TOWER_BREAK == 1){
    //printcombo(Y,0,0,0);
    return 3; 
  }
  else if(TOWER_STATE == 3){
    Tower_End = millis() - Tower_Start;
    printCombo(E, Tower_End, Total_Tower_Touch, 1); 
    return 1; 
  }
  
}


//////////////////////////////////

int performTask2(){
  SWITCH_STATE = 0;
  int Nearest_Pot_Point = 1023;
  int Breakoff_Pot_Point = 100;
  int Motor_Time_Test = 6000;
  int tying_Progress = 0;
  int Motor_Start = 0;
  int Motor_End = 0;
  KNOT_BUTTON_VAL = digitalRead(KNOT_BUTTON);
  KNOT_EYELET_VAL = digitalRead(KNOT_EYELET);
  //KNOT_LED led on pot
  //KNOT_MOTOR  servo control
  KNOT_POT_VAL = analogRead(KNOT_POT);
  KNOT_CURRENT_VAL = analogRead(KNOT_CURRENT);

  printcombo(F,KNOT_EYELET_VAL,KNOT_POT_VAL,KNOT_CURRENT_VAL);
  while(KNOT_BUTTON_VAL == 0){
    KNOT_BUTTON_VAL = digitalRead(KNOT_BUTTON);
    KNOT_EYELET_VAL = digitalRead(KNOT_EYELET);
    KNOT_POT_VAL = analogRead(KNOT_POT);
    KNOT_CURRENT_VAL = analogRead(KNOT_CURRENT);
    serialReader();
    if (Serial_String_Complete){
      SWITCH_STATE = 1;
      return 2;
      break;
    }
  }
  if(KNOT_BUTTON_VAL == 1){
    //Participant has signal he has started the task
    tying_Progress = 1; 
  }
  printcombo(G,KNOT_EYELET_VAL,KNOT_POT_VAL,KNOT_CURRENT_VAL);
  Knot_Start = millis();
  if(SWITCH_STATE == 0){
    while(KNOT_BUTTON_VAL == 0){
      KNOT_BUTTON_VAL = digitalRead(KNOT_BUTTON);
      KNOT_EYELET_VAL = digitalRead(KNOT_EYELET);
      KNOT_POT_VAL = analogRead(KNOT_POT);
      KNOT_CURRENT_VAL = analogRead(KNOT_CURRENT);
      serialReader();
      if (Serial_String_Complete){
        SWITCH_STATE = 1;
        return 2;
        break;
      }
      if(KNOT_EYELET_VAL == 1){
         digitalWrite(KNOT_LED,HIGH);
         Nearest_Pot_Point = KNOT_POT_VAL;
         printcombo(G,KNOT_EYELET_VAL,KNOT_POT_VAL,KNOT_CURRENT_VAL);
      }
      else{
        digitalWrite(KNOT_LED,LOW);
      }
    }
    Knot_End = millis() - Knot_Start;
    if(KNOT_BUTTON_VAL == 1 && SWITCH_STATE == 0){
      //Participant has signal he is done
      tying_Progress = 2; 
      printcombo(I,KNOT_EYELET_VAL,KNOT_POT_VAL,KNOT_CURRENT_VAL);
      delay(100);
      Motor_Start = millis();
    }
  }
  if(SWITCH_STATE == 0){
    while(true){
       KNOT_BUTTON_VAL = digitalRead(KNOT_BUTTON);
       KNOT_EYELET_VAL = digitalRead(KNOT_EYELET);
       KNOT_POT_VAL = analogRead(KNOT_POT);
       KNOT_CURRENT_VAL = analogRead(KNOT_CURRENT);
       printcombo(J,KNOT_EYELET_VAL,KNOT_POT_VAL,KNOT_CURRENT_VAL);
       digitalWrite(KNOT_MOTOR,HIGH);
       Motor_End = millis() - Motor_Start;
       if(Motor_End >= Motor_Time_Test){
         break; 
       }
       serialReader();
       if (Serial_String_Complete){
         SWITCH_STATE = 1;
         return 2;
         break;
       }
       if(KNOT_POT_VAL <= Breakoff_Pot_Point){
         KNOT_SNAP = 1;
         return 3;
         break;
       }
    }
    if(SWITCH_STATE == 0 && KNOT_SNAP == 0){
       Motor_End = millis() - Motor_Start;
       Knot_End = millis() - Knot_Start;
       printcombo(K,Motor_End,KNOT_POT_VAL,Knot_End);
    }
  }
  if(SWITCH_STATE == 1){
    return 2; 
  }
  else if(KNOT_SNAP == 1){
    return 3; 
  }
  else{
    return 1; 
  }
}

void serialReader(){
  Serial_Return_Val = 0;
  Serial_String_Complete = false;
  int makeSerialStringPosition;
  int inByte;
  char serialReadString[50];
  memset(&serialReadString[0], 0, sizeof(serialReadString));
  const int terminatingChar = 47; //Terminate lines with '/'

  inByte = Serial.read();
  makeSerialStringPosition=0;

  if (inByte > 0 && inByte != terminatingChar) { 
    delay(100); 
    
    while (inByte != terminatingChar && Serial.available() > 0){
      serialReadString[makeSerialStringPosition] = inByte; 
      makeSerialStringPosition++; 
      
      inByte = Serial.read(); 
    }

    if (inByte == terminatingChar)
    {
      serialReadString[makeSerialStringPosition] = 0; 
      Serial_String_Complete = true;
      if (strcmp(serialReadString, "TASK1") == 0){
        Serial_Return_Val = 1;
      }
      else if(strcmp(serialReadString, "TASK2") == 0){
        Serial_Return_Val = 2;
      }
      else if(strcmp(serialReadString, "TASK0") == 0){
        Serial_Return_Val = 9;
      }
    }
    Serial.println(serialReadString);
  }
  
}


void printCombo(char* code, unsigned long input1, unsigned long input2, unsigned long input3)
{

  Serial.print('H'); // unique header to identify start of message
  Serial.print(",");
  Serial.print(code);
  Serial.print(",");
  Serial.print(input1,DEC);
  Serial.print(",");
  Serial.print(input2,DEC);
  Serial.print(",");
  Serial.print(input3,DEC);
  Serial.print(",");
  Serial.print("Q");  // note Q is sent to mark end of line
  Serial.println();  // send a cr/lf
}

