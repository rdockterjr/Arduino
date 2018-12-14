//for motor
#include <Servo.h>

Servo Servo_Motor;

//declare pins
int RING_1_RBC = 2; //ring-base
int RING_SC = 3; //S-continuty - capactive
int RING_1_LED = 4; // LED
int RING_1_BC = 5; // Base continuity
int RING_2_RBC = 6; //ring-base
int RING_2_LED = 7; // led
int RING_2_BC = 8; // base continuity
int KNOT_EYELET = 9; // Eyelet continuity
int KNOT_BUTTON = 10; //switch for done with knot
int KNOT_LED = 11; //led on pot
int KNOT_MOTOR = 12; // servo control
int KNOT_MOTOR_SWITCH = 13; // servo current switch
int KNOT_POT = 0; //analog pin for pot

//declare pin read values
int RING_1_RBC_VAL = 0; //ring-base
int RING_SC_VAL = 0; //S-continuty
int RING_1_BC_VAL = 0; // Base continuity
int RING_2_RBC_VAL = 0; //ring-base
int RING_2_BC_VAL = 0; // base continuity
int KNOT_BUTTON_VAL = 0; //switch for done with knot
int KNOT_EYELET_VAL = 0; // Eyelet continuity
int KNOT_POT_VAL = 0; //analog pin for pot

//Initial Pins
int RING_1_RBC_INIT = 0; //ring-base
int RING_SC_INIT = 0; //S-continuty
int RING_1_BC_INIT = 0; // Base continuity
int RING_2_RBC_INIT = 0; //ring-base
int RING_2_BC_INIT = 0; // base continuity
int KNOT_BUTTON_INIT = 0; //switch for done with knot
int KNOT_EYELET_INIT = 0; // Eyelet continuity
int KNOT_POT_INIT = 0; //analog pin for pot

//declare states
int WHICH_TASK = 0;
int TOWER_STATE = 0;
int SWITCH_STATE = 0;
int TOWER_BREAK = 0;
int KNOT_SNAP = 0;


//Serial Reader
int Serial_Return_Val = 0;
boolean Serial_String_Complete = false;
int inbyte;
unsigned long serialdata;

int initError = 0;

//servo vals
int Motor_Type = 2;
//Motor is 1 for dc, 2 for servo
int Slack_End = -90;
int Force_End = 90;
int pos = 0;

//timing variables
unsigned long Tower_Start = 0;
unsigned long Tower_End = 0;
unsigned long Tower_Hit_Start = 0;
unsigned long Tower_Hit_End = 0;
unsigned long Knot_Start = 0;
unsigned long Knot_End = 0;
int Total_Tower_Touch = 0;

//Characters
//H and Q are protected
char A[] = "A";//Ring on base 1
char B[] = "B";//Ring on S 1
char C[] = "C";//Ring on second base / done
char E[] = "E";//Knot tying not started
char F[] = "F";//knot tying started
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
  pinMode(RING_1_RBC,INPUT_PULLUP);
  pinMode(RING_SC,INPUT_PULLUP);
  pinMode(RING_1_LED,OUTPUT);
  pinMode(RING_1_BC,INPUT_PULLUP);
  pinMode(RING_2_RBC,INPUT_PULLUP);
  pinMode(RING_2_LED,OUTPUT);
  pinMode(RING_2_BC,INPUT_PULLUP);
  pinMode(KNOT_BUTTON,INPUT_PULLUP);
  pinMode(KNOT_EYELET,INPUT_PULLUP);
  pinMode(KNOT_LED,OUTPUT);
  if(Motor_Type == 2){
    Servo_Motor.attach(KNOT_MOTOR); //Servo on 12
  }
  else{
    pinMode(KNOT_MOTOR,OUTPUT);
    pinMode(KNOT_MOTOR_SWITCH,OUTPUT);
  }
  //KNOT_POT is analog (default read)
  Serial.begin(9600);
  establishContact();
  if(Motor_Type == 1){
    pinMode(KNOT_MOTOR,OUTPUT);
    pinMode(KNOT_MOTOR_SWITCH,OUTPUT);
  }
  if(Motor_Type == 2){
    //relieve Torque for Servo weight
    for(pos = Force_End; pos >= Slack_End; pos -= 1) 
    {                                
      Servo_Motor.write(pos);
      delay(30);
    }
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
    if(Serial_Return_Val == 1 || WHICH_TASK == 1){
      //Task 1
      WHICH_TASK = 1;
      int ret1;
      ret1 = performTask1();
      if(ret1 == 1){
          SWITCH_STATE = 0;
        }
       if(ret1 == 2){
          printCombo(V,1,1,1);
          SWITCH_STATE = 1;
        }
        if(ret1 == 3){
          SWITCH_STATE = 0;
          printCombo(Y,0,0,0);
        } 
    }
    else if(Serial_Return_Val == 2){
      //Task 2
      WHICH_TASK = 2;
      int ret2;
      ret2 = performTask2();
      if(ret2 == 1){
        SWITCH_STATE = 0;
      }
      if(ret2 == 2){
        printCombo(V,1,1,1);
        SWITCH_STATE = 1;
      }
      if(ret2 == 3){
        printCombo(Z,0,0,0);
        SWITCH_STATE = 0;
      }
    }
    
  }
  delay(100); 
}


void establishContact() {
   if(Serial.available() <= 0) {
     RING_1_RBC_INIT = digitalRead(RING_1_RBC);
     RING_SC_INIT = digitalRead(RING_SC);
     RING_1_BC_INIT = digitalRead(RING_1_BC);
     RING_2_RBC_INIT = digitalRead(RING_2_RBC);
     RING_2_BC_INIT = digitalRead(RING_2_BC);
     KNOT_BUTTON_INIT = digitalRead(KNOT_BUTTON);
     KNOT_EYELET_INIT = digitalRead(KNOT_EYELET);
     KNOT_POT_INIT = analogRead(KNOT_POT);
     
     if(RING_1_RBC_INIT == 1){
       initError = 1;
       Serial.println("ring 1 base wires");
       digitalWrite(RING_1_LED,HIGH);
     } 
     if(RING_SC_INIT == 1){
       initError = 1;
        Serial.println("Capactiance");
        digitalWrite(RING_1_LED,HIGH);
     }
     if(RING_1_BC_INIT == 1){
       initError = 1;
        Serial.println("ring 1 base");
       digitalWrite(RING_1_LED,HIGH);
     }
     if(RING_2_RBC_INIT == 0){
       initError = 1;
        Serial.println("ring 2 base wires");
       digitalWrite(RING_2_LED,HIGH);
     }
     if(RING_2_BC_INIT == 1){
       initError = 1;
        Serial.println("ring 2 base ");
       digitalWrite(RING_2_LED,HIGH);
     }
     if(KNOT_BUTTON_INIT == 1){
       initError = 1;
        Serial.println("Knot Button");
       digitalWrite(KNOT_LED,HIGH);
     }
     if(KNOT_EYELET_INIT == 0){
       initError = 1;
        Serial.println("Knot eyelets");
       digitalWrite(KNOT_LED,HIGH);
     }
   }
   
   delay(2000);
     
  digitalWrite(KNOT_LED,LOW);
  digitalWrite(RING_1_LED,LOW);
  digitalWrite(RING_2_LED,LOW);
}

int performTask1(){
  TOWER_BREAK = 0;
  TOWER_STATE = 1;
  SWITCH_STATE = 0;
  int hasleft = 0;
  digitalWrite(RING_1_LED,LOW);
  digitalWrite(RING_2_LED,LOW);

  RING_1_RBC_VAL = digitalRead(RING_1_RBC);
  RING_SC_VAL = digitalRead(RING_SC);
  RING_1_BC_VAL = digitalRead(RING_1_BC);
  RING_2_RBC_VAL = digitalRead(RING_2_RBC);
  RING_2_BC_VAL = digitalRead(RING_2_BC);
  
  while(true){
    RING_1_RBC_VAL = digitalRead(RING_1_RBC);
    RING_SC_VAL = digitalRead(RING_SC);
    RING_1_BC_VAL = digitalRead(RING_1_BC);
    RING_2_RBC_VAL = digitalRead(RING_2_RBC);
    RING_2_BC_VAL = digitalRead(RING_2_BC);
    serialReader();
    if (Serial_String_Complete){
      SWITCH_STATE = 1;
      return 2;
      break;
    }
    if(TOWER_STATE == 1 && SWITCH_STATE == 0 && TOWER_BREAK == 0){
      if(RING_2_RBC_VAL == 0){
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
        RING_SC_VAL = digitalRead(RING_SC);
        RING_1_BC_VAL = digitalRead(RING_1_BC);
        RING_2_RBC_VAL = digitalRead(RING_2_RBC);
        RING_2_BC_VAL = digitalRead(RING_2_BC);
        while(RING_1_RBC_VAL == 0){
            digitalWrite(RING_1_LED,HIGH);
            RING_1_RBC_VAL = digitalRead(RING_1_RBC);
            RING_SC_VAL = digitalRead(RING_SC);
            RING_1_BC_VAL = digitalRead(RING_1_BC);
            RING_2_RBC_VAL = digitalRead(RING_2_RBC);
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
        digitalWrite(RING_1_LED,LOW);
        Tower_Start = millis();
        printCombo(A, Tower_Start, 1, 0); //combo A for base discontinuity
        hasleft = 1;
      }
      else if(RING_1_RBC_VAL == 1 && SWITCH_STATE == 0 && TOWER_BREAK == 0){
        //now checking ring hits
        RING_1_RBC_VAL = digitalRead(RING_1_RBC);
        RING_SC_VAL = digitalRead(RING_SC);
        RING_1_BC_VAL = digitalRead(RING_1_BC);
        RING_2_RBC_VAL = digitalRead(RING_2_RBC);
        RING_2_BC_VAL = digitalRead(RING_2_BC);
        while(RING_SC_VAL == 0){
          //While No Touch
          if(RING_2_RBC_VAL == 0){
            digitalWrite(RING_2_LED,HIGH);
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
          RING_SC_VAL = digitalRead(RING_SC);
          RING_1_BC_VAL = digitalRead(RING_1_BC);
          RING_2_RBC_VAL = digitalRead(RING_2_RBC);
          RING_2_BC_VAL = digitalRead(RING_2_BC);
          digitalWrite(RING_1_LED,LOW);
          digitalWrite(RING_2_LED,LOW);
        }
        if(SWITCH_STATE == 0 && TOWER_BREAK == 0 && TOWER_STATE == 1){
          //Make sure that the break from continuity was not for other reasons
          printCombo(B, 0, 1, 0); //combo B for ring continuity
          Total_Tower_Touch += 1;
          Tower_Hit_Start = millis();
          delay(10);
          RING_1_RBC_VAL = digitalRead(RING_1_RBC);
          RING_SC_VAL = digitalRead(RING_SC);
          RING_1_BC_VAL = digitalRead(RING_1_BC);
          RING_2_RBC_VAL = digitalRead(RING_2_RBC);
          RING_2_BC_VAL = digitalRead(RING_2_BC);
          while(RING_SC_VAL == 1){
            //While touching
            RING_1_RBC_VAL = digitalRead(RING_1_RBC);
            RING_SC_VAL = digitalRead(RING_SC);
            RING_1_BC_VAL = digitalRead(RING_1_BC);
            RING_2_RBC_VAL = digitalRead(RING_2_RBC);
            RING_2_BC_VAL = digitalRead(RING_2_BC);
            if(RING_2_RBC_VAL == 0){
              digitalWrite(RING_2_LED,HIGH);
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
            digitalWrite(RING_2_LED,HIGH);
          }
          if(SWITCH_STATE == 0 && TOWER_BREAK == 0 && TOWER_STATE == 1){
            Tower_Hit_End = millis() - Tower_Hit_Start;
            printCombo(B, Tower_Hit_End, 0, 0); //combo B for ring continuity
          }
        }
        else{
         Tower_End = millis() - Tower_Start;
         break; 
         
        }
      }
    }
    if(SWITCH_STATE == 1 || TOWER_BREAK == 1 || TOWER_STATE == 2){
      Tower_End = millis() - Tower_Start;
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
  else if(TOWER_STATE == 2){
    Serial.flush();
    delay(1000);
    printCombo(C, Tower_End, Total_Tower_Touch, 1); 
    delay(6000);
    digitalWrite(RING_1_LED,LOW);
    digitalWrite(RING_2_LED,LOW);
    return 1; 
  }

}


//////////////////////////////////

int performTask2(){

  SWITCH_STATE = 0;
  double Nearest_Pot_Point = 1023;
  int Breakoff_Pot_Point = 100;
  double Furthest_Seperation = 1023;
  int switch_back = 2;
  int Motor_Time_Test_1 = 3000;
  int Motor_Time_Test_2 = 9000;
  int tying_Progress = 0;
  int Motor_Start = 0;
  int Motor_End = 0;
  int Release_Start = 0;
  int Release_End = 0;
  int Release_Time = 500;
  digitalWrite(KNOT_MOTOR_SWITCH,HIGH);
  delay(10);
  digitalWrite(KNOT_MOTOR,LOW);
  digitalWrite(KNOT_LED,LOW);
  KNOT_BUTTON_VAL = digitalRead(KNOT_BUTTON);
  KNOT_EYELET_VAL = digitalRead(KNOT_EYELET);
  //KNOT_LED led on pot
  //KNOT_MOTOR  servo control
  KNOT_POT_VAL = analogRead(KNOT_POT);

  printCombo(E,KNOT_EYELET_VAL,KNOT_POT_VAL,0);
  while(KNOT_BUTTON_VAL == 0){
    //Waiting to start
    KNOT_BUTTON_VAL = digitalRead(KNOT_BUTTON);
    KNOT_EYELET_VAL = digitalRead(KNOT_EYELET);
    KNOT_POT_VAL = analogRead(KNOT_POT);
    serialReader();
    if (Serial_String_Complete){
      SWITCH_STATE = 1;
      return 2;
      break;
    }
  }
  int bounce_time = millis();
  int bounce_prevent;
  if(KNOT_BUTTON_VAL == 1){
    printCombo(F,KNOT_EYELET_VAL,KNOT_POT_VAL,0);
    //Participant has signal he has started the task
    tying_Progress = 1; 
  }
  Knot_Start = millis();
  if(SWITCH_STATE == 0){
    bounce_prevent = millis() - bounce_time;
    while(true){
      bounce_prevent = millis() - bounce_time;
      //waiting for participant to signal he is done
      KNOT_BUTTON_VAL = digitalRead(KNOT_BUTTON);
      KNOT_EYELET_VAL = digitalRead(KNOT_EYELET);
      KNOT_POT_VAL = analogRead(KNOT_POT);
      if(KNOT_BUTTON_VAL == 1 && bounce_prevent >= 5000){
       break; 
      }
      serialReader();
      if (Serial_String_Complete){
        SWITCH_STATE = 1;
        return 2;
        break;
      }
      if(KNOT_EYELET_VAL == 0){
         digitalWrite(KNOT_LED,HIGH);
         Nearest_Pot_Point = (double) KNOT_POT_VAL;
         if(switch_back == 1){
           printCombo(G,1,KNOT_POT_VAL,0);
           switch_back = 2;
         }
      }
      else{
        if(switch_back == 2){
           printCombo(G,0,KNOT_POT_VAL,0);
           switch_back = 1;
         }
        digitalWrite(KNOT_LED,LOW);
      }
    }
    if(SWITCH_STATE == 0){
      //Participant has signal he is done
      Knot_End = millis() - Knot_Start;
      tying_Progress = 2; 
      printCombo(I,KNOT_EYELET_VAL,KNOT_POT_VAL,0);
      digitalWrite(KNOT_LED,LOW);
      delay(100);
    }
  }
  digitalWrite(KNOT_LED,LOW);
  delay(1000);
  if(SWITCH_STATE == 0){
    if(Motor_Type == 1){
      Motor_Start = millis();
      digitalWrite(KNOT_MOTOR_SWITCH,HIGH);
      delay(10);
      digitalWrite(KNOT_MOTOR,HIGH);
      while(true){
        //Applying force using Torque motor
         KNOT_BUTTON_VAL = digitalRead(KNOT_BUTTON);
         KNOT_EYELET_VAL = digitalRead(KNOT_EYELET);
         KNOT_POT_VAL = analogRead(KNOT_POT);
         Motor_End = millis() - Motor_Start;
         if(Motor_End >= Motor_Time_Test_1){
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
         if(KNOT_POT_VAL <= Furthest_Seperation){
           printCombo(J,KNOT_EYELET_VAL,KNOT_POT_VAL,1);
           Furthest_Seperation = (double) KNOT_POT_VAL;
         }
         if(KNOT_EYELET_VAL == 0){
             digitalWrite(KNOT_LED,HIGH);
          }
          else{
            digitalWrite(KNOT_LED,LOW);
          }
      }
      digitalWrite(KNOT_MOTOR,LOW);
      digitalWrite(KNOT_LED,LOW);
    }
    if(Motor_Type == 2){
      Motor_Start = millis();
      printCombo(J,KNOT_EYELET_VAL,KNOT_POT_VAL,1);
      for(pos = Slack_End; pos < Force_End; pos += 1) 
      {                                
        Servo_Motor.write(pos);
        delay(15);
      }
      while(true){
        //Applying force using servo
         KNOT_BUTTON_VAL = digitalRead(KNOT_BUTTON);
         KNOT_EYELET_VAL = digitalRead(KNOT_EYELET);
         KNOT_POT_VAL = analogRead(KNOT_POT);

         Motor_End = millis() - Motor_Start;
         if(Motor_End >= Motor_Time_Test_2){
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
         if(KNOT_POT_VAL <= Furthest_Seperation){
           printCombo(J,KNOT_EYELET_VAL,KNOT_POT_VAL,1);
           Furthest_Seperation = (double) KNOT_POT_VAL;
         }
         if(KNOT_EYELET_VAL == 0){
             digitalWrite(KNOT_LED,HIGH);
          }
          else{
            digitalWrite(KNOT_LED,LOW);
          }
         delay(50);
      }
    }
    digitalWrite(KNOT_LED,LOW);
    delay(10);
    if(Motor_Type == 1){
      //relieve Tension for DC Motor
      digitalWrite(KNOT_MOTOR_SWITCH,LOW);
      delay(10);
      digitalWrite(KNOT_MOTOR,HIGH);
      Release_Start = millis();
      while(true){
        //Applying force using Torque motor
         KNOT_BUTTON_VAL = digitalRead(KNOT_BUTTON);
         KNOT_EYELET_VAL = digitalRead(KNOT_EYELET);
         KNOT_POT_VAL = analogRead(KNOT_POT);
         printCombo(J,KNOT_EYELET_VAL,KNOT_POT_VAL,1);
         Release_End = millis() - Release_Start;
         if(Release_End >= Release_Time){
           break; 
         }
      }
      digitalWrite(KNOT_MOTOR,LOW);
    }
    if(Motor_Type == 2){
      //relieve Torque for Servo weight
      for(pos = Force_End; pos >= Slack_End; pos -= 1) 
      {                                
        Servo_Motor.write(pos);
        delay(30);
      }
    }
    if(SWITCH_STATE == 0 && KNOT_SNAP == 0){
       printCombo(K,Furthest_Seperation,Nearest_Pot_Point,Knot_End);
    }
  }
  if(SWITCH_STATE == 1){
    return 2; 
  }
  else if(KNOT_SNAP == 1){
    if(Motor_Type == 2){
      //relieve Torque for Servo weight
      for(pos = Force_End; pos >= Slack_End; pos -= 1) 
      {                                
        Servo_Motor.write(pos);
        delay(30);
      }
    }
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
        WHICH_TASK = 1;
      }
      else if(strcmp(serialReadString, "TASK2") == 0){
        Serial_Return_Val = 2;
        WHICH_TASK = 2;
      }
      else if(strcmp(serialReadString, "TASK0") == 0){
        Serial_Return_Val = 9;
      }
    }
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

