//for motor
#include <Servo.h>
#include <SD.h>
#include <string.h>
#include <stdlib.h>

//Data Logging initializers
File SD_Print_File;
String Folder_Name = "/FRS_DATA/";
String ParticipantID = "";
int Task1Try = 1;
int Task2Try = 1;
//Initials plus 3 digit code eg: RD123
boolean SD_Initialized = false;
boolean Folder_Created = false;
boolean File_Created = false;
boolean ID_Received = false;
unsigned long Time_Stamp = 0;
int Task1Int = 1;
int Task2Int = 2;

Servo Servo_Motor;

//declare pins
//For SD need jumpers
//pin11->pin51
//pin12->pin50
//pin13->pin52
int RESET_PIN = 9;
int ERROR_LED = 2; //Flashses for Errors
int RING_1_RBC = 22; //ring-base-
int RING_SC = 23; //S-continuty - capactive
int RING_1_LED = 24; // LED
int RING_1_BC = 25; // Base continuity
int RING_2_RBC = 26; //ring-base
int RING_2_LED = 27; // led
int RING_2_BC = 28; // base continuity
int RING_3_RBC = 29; //ring-base
int RING_3_LED = 30; // led
int RING_3_BC = 31; // base continuity
int RING_4_RBC = 32; //ring-base
int RING_4_LED = 33; // led
int RING_4_BC = 34; // base continuity

int KNOT_BUTTON = 40; //switch for done with knot
int KNOT_EYELET = 46; // Eyelet continuity (from eyelet)
int KNOT_LED = 42; //led on pot
int KNOT_MOTOR = 44; // servo control
int KNOT_POT = 14; //analog pin for pot

//declare pin read values
int RING_1_RBC_VAL = 0; //ring-base
int RING_SC_VAL = 0; //S-continuty
int RING_1_BC_VAL = 0; // Base continuity
int RING_2_RBC_VAL = 0; //ring-base
int RING_2_BC_VAL = 0; // base continuity
int RING_3_RBC_VAL = 0; //ring-base
int RING_3_BC_VAL = 0; // base continuity
int RING_4_RBC_VAL = 0; //ring-base
int RING_4_BC_VAL = 0; // base continuity
int KNOT_BUTTON_VAL = 0; //switch for done with knot
int KNOT_EYELET_VAL = 0; // Eyelet continuity
int KNOT_POT_VAL = 0; //analog pin for pot

//Initial Pins
int RING_1_RBC_INIT = 0; //ring-base
int RING_SC_INIT = 0; //S-continuty
int RING_1_BC_INIT = 0; // Base continuity
int RING_2_RBC_INIT = 0; //ring-base
int RING_2_BC_INIT = 0; // base continuity
int RING_3_RBC_INIT = 0; //ring-base
int RING_3_BC_INIT = 0; // base continuity
int RING_4_RBC_INIT = 0; //ring-base
int RING_4_BC_INIT = 0; // base continuity
int KNOT_BUTTON_INIT = 0; //switch for done with knot
int KNOT_EYELET_INIT = 0; // Eyelet continuity
int KNOT_POT_INIT = 0; //analog pin for pot

//declare states
int WHICH_TASK = 0;
int SWITCH_STATE = 0;
int KNOT_SNAP = 0;


//Serial Reader
int Serial_Return_Val = 0;
boolean Serial_String_Complete = false;
int inbyte;
unsigned long serialdata;

int initError = 0;

//servo vals
int Slack_End = 0;
int Force_End = 90;
int pos = 0;

//timing variables
unsigned long Tower_Start = 0;
unsigned long Tower_End = 0;
unsigned long Tower_Time_Stamp_Start = 0;
unsigned long Tower_Time_Stamp = 0;
unsigned long Tower_Hit_Start = 0;
unsigned long Tower_Hit_End = 0;
unsigned long Tower_Touch_Time = 0;
unsigned long Knot_Start = 0;
unsigned long Knot_End = 0;
unsigned long Knot_Time_Stamp_Start = 0;
unsigned long Knot_Time_Stamp = 0;
int Total_Tower_Touch = 0;

//Characters
//H and Q are protected
char A[] = "A";//Ring Task Started
char B[] = "B";//Ring Task Finished
char C[] = "C";//Knot Task Started
char D[] = "D";//Knot Task Finished
char Y[] = "Y";//Auto Fail for ring tower
char Z[] = "Z";//Auto Fail for knot

void setup()
{
  //setup pins
  digitalWrite(RESET_PIN, HIGH); //Declare reset pin high (must come first)
  pinMode(RESET_PIN,OUTPUT);     //Declare resetpin as output
  pinMode(53,OUTPUT); //Required for SD
  //Pins 50, 51, 11, 12, 13 reserved for SD
  pinMode(ERROR_LED,OUTPUT);
  pinMode(RING_1_RBC,INPUT_PULLUP);
  pinMode(RING_SC,INPUT_PULLUP);
  pinMode(RING_1_LED,OUTPUT);
  pinMode(RING_1_BC,INPUT_PULLUP);
  pinMode(RING_2_RBC,INPUT_PULLUP);
  pinMode(RING_2_LED,OUTPUT);
  pinMode(RING_2_BC,INPUT_PULLUP);
  pinMode(RING_3_RBC,INPUT_PULLUP);
  pinMode(RING_3_LED,OUTPUT);
  pinMode(RING_3_BC,INPUT_PULLUP);
  pinMode(RING_4_RBC,INPUT_PULLUP);
  pinMode(RING_4_LED,OUTPUT);
  pinMode(RING_4_BC,INPUT_PULLUP);
  
  pinMode(KNOT_BUTTON,INPUT_PULLUP);
  pinMode(KNOT_EYELET,INPUT_PULLUP);
  pinMode(KNOT_LED,OUTPUT);
  //Setup Motors
  Servo_Motor.attach(KNOT_MOTOR); //Servo on 12
  
  //relieve Torque for Servo weight
  for(pos = Force_End; pos >= Slack_End; pos -= 1) 
  {                                
    Servo_Motor.write(pos);
    delay(30);
  }
 
  Serial.begin(9600);
  while(!Serial){
  }
  serialReader();
  while(!ID_Received){
    serialReader();
  }
  
  
  if(!SD.begin(8)){
   Serial.println("SD failed to Initialize");
   return; 
  }
  else{
    SD_Initialized = true;
  }
  
  establishContact();
  
  
  create_folder();
  delay(2000);
  Serial.flush();
}

//TASK NUMBERS
//TASK 1: Ring tower transfer
//TASK 2: Knot Tying
//....

void loop()
{

  serialReader();
  if (Serial_String_Complete || SWITCH_STATE == 1){
    SWITCH_STATE = 0;
    if(Serial_Return_Val == 1 || WHICH_TASK == 1){
      //Task 1 Ring Tower
      WHICH_TASK = 1;
      int ret1;
      create_file(Task1Int);
      if(File_Created){
        ret1 = performTask1();
      }
      if(ret1 == 1){
        SWITCH_STATE = 0;
        delay(1000);
        digitalWrite(RING_1_LED,LOW);
        digitalWrite(RING_2_LED,LOW); 
        digitalWrite(RING_3_LED,LOW);
        digitalWrite(RING_4_LED,LOW);
        Serial_String_Complete = false;
        Serial_Return_Val = 0;
      }
      if(ret1 == 2){
        PrintToFile(9999,9999,9999,9999,9999,9999);
        SD_Print_File.close();
        SWITCH_STATE = 1;
        digitalWrite(RING_1_LED,HIGH);
        digitalWrite(RING_2_LED,HIGH);
        digitalWrite(RING_3_LED,HIGH);
        digitalWrite(RING_4_LED,HIGH);
        delay(1000);
        digitalWrite(RING_1_LED,LOW);
        digitalWrite(RING_2_LED,LOW);
        digitalWrite(RING_3_LED,LOW);
        digitalWrite(RING_4_LED,LOW);
      }
      if(ret1 == 3){
        PrintToFile(9999,9999,9999,9999,9999,9999);
        SD_Print_File.close();
        SWITCH_STATE = 0;
        PrintToSerial(Y,0,0,0);
        digitalWrite(RING_1_LED,HIGH);
        digitalWrite(RING_2_LED,HIGH);
        digitalWrite(RING_3_LED,HIGH);
        digitalWrite(RING_4_LED,HIGH);
        delay(1000);
        digitalWrite(RING_1_LED,LOW);
        digitalWrite(RING_2_LED,LOW);
        digitalWrite(RING_3_LED,LOW);
        digitalWrite(RING_4_LED,LOW);
        Serial_String_Complete = false;
        Serial_Return_Val = 0;
      } 
    }
    else if(Serial_Return_Val == 2){
      //Task 2 Knot Tying
      WHICH_TASK = 2;
      int ret2;
      create_file(Task2Int);
      if(File_Created){
        ret2 = performTask2();
      }
      if(ret2 == 1){
        SWITCH_STATE = 0;
        Serial_String_Complete = false;
        Serial_Return_Val = 0;
      }
      if(ret2 == 2){
        PrintToFile(9999,9999,9999,9999,9999,9999);
        SD_Print_File.close();
        SWITCH_STATE = 1;

        //relieve Torque for Servo weight
        for(pos = Force_End; pos >= Slack_End; pos -= 1) 
        {                                
          Servo_Motor.write(pos);
          delay(30);
        }
      }
      if(ret2 == 3){
        PrintToFile(9999,9999,9999,9999,9999,9999);
        SD_Print_File.close();
        PrintToSerial(Z,0,0,0);
        SWITCH_STATE = 0;
        Serial_String_Complete = false;
        Serial_Return_Val = 0;
        //relieve Torque for Servo weight
        for(pos = Force_End; pos >= Slack_End; pos -= 1) 
        {                                
          Servo_Motor.write(pos);
          delay(30);
        }
      }
    }

  }
  
  SD_Print_File.close();
  delay(100); 
}


void establishContact() {
   int initerror1 = 0;
   int initerror2 = 0;
   int initerror3 = 0;
   int initerror4 = 0;
   int initerror5 = 0;
   if(true){
     RING_1_RBC_INIT = digitalRead(RING_1_RBC);
     RING_SC_INIT = digitalRead(RING_SC);
     RING_1_BC_INIT = digitalRead(RING_1_BC);
     RING_2_RBC_INIT = digitalRead(RING_2_RBC);
     RING_2_BC_INIT = digitalRead(RING_2_BC);
     RING_3_RBC_INIT = digitalRead(RING_3_RBC);
     RING_3_BC_INIT = digitalRead(RING_3_BC);
     RING_4_RBC_INIT = digitalRead(RING_4_RBC);
     RING_4_BC_INIT = digitalRead(RING_4_BC);
     KNOT_BUTTON_INIT = digitalRead(KNOT_BUTTON);
     KNOT_EYELET_INIT = digitalRead(KNOT_EYELET);
     KNOT_POT_INIT = analogRead(KNOT_POT);
     
     if(RING_1_RBC_INIT == 1){
       initerror1 = 1;
       //Serial.println("ring 1 base wires");
     } 
     if(RING_SC_INIT == 1){
       initerror1 = 1;
       //Serial.println("Capacitance");
     }
     if(RING_1_BC_INIT == 1){
       initerror1 = 1;
       //Serial.println("ring 1 base");
     }
     if(RING_2_RBC_INIT == 0){
       initerror2 = 1;
       //Serial.println("ring 2 base wires");
     }
     if(RING_2_BC_INIT == 1){
       initerror2 = 1;
        //Serial.println("ring 2 base ");
     }
     if(RING_3_RBC_INIT == 1){
       initerror3 = 1;
       // Serial.println("ring 3 base wires");
     }
     if(RING_3_BC_INIT == 1){
       initerror3 = 1;
       // Serial.println("ring 3 base ");
     }
     if(RING_4_RBC_INIT == 0){
       initerror4 = 1;
      // Serial.println("ring 4 base wires ");
     }
     if(RING_4_BC_INIT == 1){
       initerror4 = 1;
       //Serial.println("ring 4 base ");
     }
     
     if(KNOT_BUTTON_INIT == 1){
       initerror5 = 1;
      // Serial.println("Knot Button");
     }
     if(KNOT_EYELET_INIT == 0){
       initerror5 = 1;
      // Serial.println("Knot eyelets");
     }
   }
   int yy = 0;
   if(initerror1 == 1){
     for(yy = 0; yy < 1; yy++){
       delay(500);
       digitalWrite(ERROR_LED,HIGH);
       delay(500);
       digitalWrite(ERROR_LED,LOW);
     }
     delay(2000);
  }
  
  if(initerror2 == 1){
     for(yy = 0; yy < 2; yy++){
       delay(500);
       digitalWrite(ERROR_LED,HIGH);
       delay(500);
       digitalWrite(ERROR_LED,LOW);
     }
  }
  delay(2000);
  if(initerror3 == 1){
     for(yy = 0; yy < 3; yy++){
       delay(500);
       digitalWrite(ERROR_LED,HIGH);
       delay(500);
       digitalWrite(ERROR_LED,LOW);
     }
     delay(2000);
  }
  
  if(initerror4 == 1){
     for(yy = 0; yy < 4; yy++){
       delay(500);
       digitalWrite(ERROR_LED,HIGH);
       delay(500);
       digitalWrite(ERROR_LED,LOW);
     }
     delay(2000);
  }
  
  if(initerror5 == 1){
     for(yy = 0; yy < 5; yy++){
       delay(500);
       digitalWrite(ERROR_LED,HIGH);
       delay(500);
       digitalWrite(ERROR_LED,LOW);
     }
  }
   delay(2000);
}

int performTask1(){
  Tower_Start = 0;
  Tower_End = 0;
  Tower_Time_Stamp_Start = 0;
  Tower_Time_Stamp = 0;
  Tower_Hit_Start = 0;
  Tower_Hit_End = 0;
  Tower_Touch_Time = 0;
  Tower_Time_Stamp_Start = millis();
  Total_Tower_Touch = 0;
  
  PrintToSerial(A, 0, 0, 0);
  int FIRST_TOWER = 0;
  int hasswitched = 0;
  digitalWrite(RING_1_LED,LOW);
  digitalWrite(RING_2_LED,LOW);
  digitalWrite(RING_3_LED,LOW);
  digitalWrite(RING_4_LED,LOW);

  RING_1_RBC_VAL = digitalRead(RING_1_RBC);
  RING_SC_VAL = digitalRead(RING_SC);
  RING_1_BC_VAL = digitalRead(RING_1_BC);
  RING_2_RBC_VAL = digitalRead(RING_2_RBC);
  RING_2_BC_VAL = digitalRead(RING_2_BC);
  RING_3_RBC_VAL = digitalRead(RING_3_RBC);
  RING_3_BC_VAL = digitalRead(RING_3_BC);
  RING_4_RBC_VAL = digitalRead(RING_4_RBC);
  RING_4_BC_VAL = digitalRead(RING_4_BC);
  
  Tower_Time_Stamp = millis() - Tower_Time_Stamp_Start;
  PrintToFile(Tower_Time_Stamp,1,1,RING_SC_VAL,Total_Tower_Touch,0);
  
  while(true){
    //wait for ring to be on the two starting towers
    RING_1_RBC_VAL = digitalRead(RING_1_RBC);
    RING_SC_VAL = digitalRead(RING_SC);
    RING_1_BC_VAL = digitalRead(RING_1_BC);
    RING_2_RBC_VAL = digitalRead(RING_2_RBC);
    RING_2_BC_VAL = digitalRead(RING_2_BC);
    RING_3_RBC_VAL = digitalRead(RING_3_RBC);
    RING_3_BC_VAL = digitalRead(RING_3_BC);
    RING_4_RBC_VAL = digitalRead(RING_4_RBC);
    RING_4_BC_VAL = digitalRead(RING_4_BC);
    if(RING_1_RBC_VAL == 0 && RING_3_RBC_VAL == 0){
      break;
    }
    digitalWrite(RING_1_LED,HIGH);
    digitalWrite(RING_2_LED,HIGH);
    digitalWrite(RING_3_LED,HIGH);
    digitalWrite(RING_4_LED,HIGH);
    
    serialReader();
    if (Serial_String_Complete){
      return 2;
    }
    Tower_Time_Stamp = millis() - Tower_Time_Stamp_Start;
    PrintToFile(Tower_Time_Stamp,0,9,RING_SC_VAL,Total_Tower_Touch,0);
    
    if(RING_1_BC_VAL == 1 || RING_2_BC_VAL == 1 || RING_3_BC_VAL == 1 || RING_4_BC_VAL == 1){
        //Tower Break off - Fail
        return 3;
    }
  }
  
  digitalWrite(RING_1_LED,LOW);
  digitalWrite(RING_2_LED,LOW);
  digitalWrite(RING_3_LED,LOW);
  digitalWrite(RING_4_LED,LOW);
  
  while(true){
    //wait for ring to leave starting tower
    RING_1_RBC_VAL = digitalRead(RING_1_RBC);
    RING_SC_VAL = digitalRead(RING_SC);
    RING_1_BC_VAL = digitalRead(RING_1_BC);
    RING_2_RBC_VAL = digitalRead(RING_2_RBC);
    RING_2_BC_VAL = digitalRead(RING_2_BC);
    RING_3_RBC_VAL = digitalRead(RING_3_RBC);
    RING_3_BC_VAL = digitalRead(RING_3_BC);
    RING_4_RBC_VAL = digitalRead(RING_4_RBC);
    RING_4_BC_VAL = digitalRead(RING_4_BC);
    digitalWrite(RING_1_LED,HIGH);
    digitalWrite(RING_3_LED,HIGH);

    serialReader();
    if (Serial_String_Complete){
      return 2;
    }
    Tower_Time_Stamp = millis() - Tower_Time_Stamp_Start;
    PrintToFile(Tower_Time_Stamp,1,1,RING_SC_VAL,Total_Tower_Touch,0);
    
    if(RING_1_BC_VAL == 1 || RING_2_BC_VAL == 1 || RING_3_BC_VAL == 1 || RING_4_BC_VAL == 1){
        //Tower Break off - Fail
        return 3;
    }

    if(RING_1_RBC_VAL == 1 || RING_3_RBC_VAL == 1){
      //check if ring has left
      if(RING_1_RBC_VAL == 1){
        FIRST_TOWER = 1;
      }
      if(RING_3_RBC_VAL == 1){
        FIRST_TOWER = 3;
      }
      break;
    }
  }
  
  //////Start Timer and look for touches////////
  Tower_Start = millis();
  digitalWrite(RING_1_LED,LOW);
  digitalWrite(RING_3_LED,LOW);
  Tower_Time_Stamp = millis() - Tower_Time_Stamp_Start;
  PrintToFile(Tower_Time_Stamp,2,3,RING_SC_VAL,Total_Tower_Touch,0);
  
  while(true){
    //Now Checking for Touches
    while(true){
      //while ring not touching
      RING_1_RBC_VAL = digitalRead(RING_1_RBC);
      RING_SC_VAL = digitalRead(RING_SC);
      RING_1_BC_VAL = digitalRead(RING_1_BC);
      RING_2_RBC_VAL = digitalRead(RING_2_RBC);
      RING_2_BC_VAL = digitalRead(RING_2_BC);
      RING_3_RBC_VAL = digitalRead(RING_3_RBC);
      RING_3_BC_VAL = digitalRead(RING_3_BC);
      RING_4_RBC_VAL = digitalRead(RING_4_RBC);
      RING_4_BC_VAL = digitalRead(RING_4_BC);
      
      Tower_Time_Stamp = millis() - Tower_Time_Stamp_Start;
      if(hasswitched == 0){
        PrintToFile(Tower_Time_Stamp,2,3,RING_SC_VAL,Total_Tower_Touch,Tower_Touch_Time);
      }
      else{
        PrintToFile(Tower_Time_Stamp,4,2,RING_SC_VAL,Total_Tower_Touch,Tower_Touch_Time);
      }
      
      serialReader();
      if (Serial_String_Complete){
        return 2;
      }
      
      //Check landings on towerss
      if(FIRST_TOWER == 1){
        if(RING_2_RBC_VAL == 0 && hasswitched == 0){
          hasswitched = 1;
          PrintToFile(Tower_Time_Stamp,3,2,RING_SC_VAL,Total_Tower_Touch,Tower_Touch_Time);
          digitalWrite(RING_2_LED,HIGH);
          digitalWrite(RING_3_LED,HIGH);
          delay(1000);
          digitalWrite(RING_2_LED,LOW);
          digitalWrite(RING_3_LED,LOW);
        }
        if(RING_4_RBC_VAL == 0){
          if(hasswitched = 1){
            Tower_End = millis() - Tower_Start;
            Tower_Time_Stamp = millis() - Tower_Time_Stamp_Start;
            PrintToFile(Tower_Time_Stamp,5,4,RING_SC_VAL,Total_Tower_Touch,Tower_Touch_Time);
            PrintToSerial(B, Tower_End, Total_Tower_Touch, Tower_Touch_Time); 
            digitalWrite(RING_1_LED,HIGH);
            digitalWrite(RING_2_LED,HIGH); 
            digitalWrite(RING_3_LED,HIGH);
            digitalWrite(RING_4_LED,HIGH);
            return 1;
          }
        }
      }
      if(FIRST_TOWER == 3){
        if(RING_4_RBC_VAL == 0 && hasswitched == 0){
          hasswitched = 1;
          PrintToFile(Tower_Time_Stamp,3,2,RING_SC_VAL,Total_Tower_Touch,Tower_Touch_Time);
          digitalWrite(RING_1_LED,HIGH);
          digitalWrite(RING_4_LED,HIGH);
          delay(1000);
          digitalWrite(RING_1_LED,LOW);
          digitalWrite(RING_4_LED,LOW);
        }
        if(RING_2_RBC_VAL == 0){
          if(hasswitched = 1){
            Tower_End = millis() - Tower_Start;
            Tower_Time_Stamp = millis() - Tower_Time_Stamp_Start;
            PrintToFile(Tower_Time_Stamp,5,4,RING_SC_VAL,Total_Tower_Touch,Tower_Touch_Time);
            PrintToSerial(B, Tower_End, Total_Tower_Touch, Tower_Touch_Time);
            digitalWrite(RING_1_LED,HIGH);
            digitalWrite(RING_2_LED,HIGH); 
            digitalWrite(RING_3_LED,HIGH);
            digitalWrite(RING_4_LED,HIGH);
            return 1;
          }
        }
      }
      //check after base checks to avoid extra count
      if(RING_SC_VAL == 1){
       Tower_Hit_Start = millis();
       Total_Tower_Touch = Total_Tower_Touch + 1;
       digitalWrite(RING_1_LED,HIGH);
       digitalWrite(RING_2_LED,HIGH);
       digitalWrite(RING_3_LED,HIGH);
       digitalWrite(RING_4_LED,HIGH);
       break; 
      }
      if(RING_1_BC_VAL == 1 || RING_2_BC_VAL == 1 || RING_3_BC_VAL == 1 || RING_4_BC_VAL == 1){
        //Tower Break off - Fail
        return 3;
      }
    }
    
    while(true){
      //During touch of S Wire
      RING_1_RBC_VAL = digitalRead(RING_1_RBC);
      RING_SC_VAL = digitalRead(RING_SC);
      RING_1_BC_VAL = digitalRead(RING_1_BC);
      RING_2_RBC_VAL = digitalRead(RING_2_RBC);
      RING_2_BC_VAL = digitalRead(RING_2_BC);
      RING_3_RBC_VAL = digitalRead(RING_3_RBC);
      RING_3_BC_VAL = digitalRead(RING_3_BC);
      RING_4_RBC_VAL = digitalRead(RING_4_RBC);
      RING_4_BC_VAL = digitalRead(RING_4_BC);
      
      Tower_Time_Stamp = millis() - Tower_Time_Stamp_Start;
      if(hasswitched == 0){
        PrintToFile(Tower_Time_Stamp,2,3,RING_SC_VAL,Total_Tower_Touch,Tower_Touch_Time);
      }
      else{
        PrintToFile(Tower_Time_Stamp,4,2,RING_SC_VAL,Total_Tower_Touch,Tower_Touch_Time);
      }
      
      serialReader();
      if (Serial_String_Complete){
        return 2;
      }
      
      //Check landings on towerss
      if(FIRST_TOWER == 1){
        if(RING_2_RBC_VAL == 0 && hasswitched == 0){
          hasswitched = 1;
          PrintToFile(Tower_Time_Stamp,3,2,RING_SC_VAL,Total_Tower_Touch,Tower_Touch_Time);
          digitalWrite(RING_2_LED,HIGH);
          digitalWrite(RING_3_LED,HIGH);
          delay(1000);
          break;
        }
        if(RING_4_RBC_VAL == 0){
          if(hasswitched = 1){
            Tower_End = millis() - Tower_Start;
            Tower_Time_Stamp = millis() - Tower_Time_Stamp_Start;
            PrintToFile(Tower_Time_Stamp,5,4,RING_SC_VAL,Total_Tower_Touch,Tower_Touch_Time);
            PrintToSerial(B, Tower_End, Total_Tower_Touch, Tower_Touch_Time); 
            digitalWrite(RING_1_LED,HIGH);
            digitalWrite(RING_2_LED,HIGH); 
            digitalWrite(RING_3_LED,HIGH);
            digitalWrite(RING_4_LED,HIGH);
            return 1;
          }
        }
      }
      if(FIRST_TOWER == 3){
        if(RING_4_RBC_VAL == 0 && hasswitched == 0){
          hasswitched = 1;
          PrintToFile(Tower_Time_Stamp,3,2,RING_SC_VAL,Total_Tower_Touch,Tower_Touch_Time);
          digitalWrite(RING_1_LED,HIGH);
          digitalWrite(RING_4_LED,HIGH);
          break;
          delay(1000);
        }
        if(RING_2_RBC_VAL == 0){
          if(hasswitched = 1){
            Tower_End = millis() - Tower_Start;
            Tower_Time_Stamp = millis() - Tower_Time_Stamp_Start;
            PrintToFile(Tower_Time_Stamp,5,4,RING_SC_VAL,Total_Tower_Touch,Tower_Touch_Time);
            PrintToSerial(B, Tower_End, Total_Tower_Touch, Tower_Touch_Time);
            digitalWrite(RING_1_LED,HIGH);
            digitalWrite(RING_2_LED,HIGH); 
            digitalWrite(RING_3_LED,HIGH);
            digitalWrite(RING_4_LED,HIGH);
            return 1;
          }
        }
      }
      //check after base checks to avoid extra count
      if(RING_SC_VAL == 0){
       Tower_Hit_End = millis() - Tower_Hit_Start;
       Tower_Touch_Time += Tower_Hit_End;
       Tower_Hit_Start = 0;
       digitalWrite(RING_1_LED,LOW);
       digitalWrite(RING_2_LED,LOW);
       digitalWrite(RING_3_LED,LOW);
       digitalWrite(RING_4_LED,LOW);
       break; 
      }
      if(RING_1_BC_VAL == 1 || RING_2_BC_VAL == 1 || RING_3_BC_VAL == 1 || RING_4_BC_VAL == 1){
        //Tower Break off - Fail
        return 3;
      }
    }
  }
  
  //Done
}


//////////////////////////////////

int performTask2(){
  //Initialize
  Knot_Time_Stamp_Start = millis();
  SWITCH_STATE = 0;
  double Nearest_Pot_Point = 998;
  int Breakoff_Pot_Point = 100;
  double Furthest_Seperation = 1023;
  int switch_back = 2;
  unsigned long Motor_Time_Test = 6000;
  unsigned long tying_Progress = 0;
  unsigned long Motor_Start = 0;
  unsigned long Motor_End = 0;
  unsigned long Release_Start = 0;
  unsigned long Release_End = 0;
  unsigned long Release_Time = 200;
  
  //check reads
  digitalWrite(KNOT_MOTOR,LOW);
  digitalWrite(KNOT_LED,LOW);
  KNOT_BUTTON_VAL = digitalRead(KNOT_BUTTON);
  KNOT_EYELET_VAL = digitalRead(KNOT_EYELET);
  //KNOT_LED led on pot
  //KNOT_MOTOR  servo control
  KNOT_POT_VAL = analogRead(KNOT_POT);
  
  
  //print initials
  Knot_Time_Stamp = millis() - Knot_Time_Stamp_Start;
  PrintToFile(Knot_Time_Stamp,1,1-KNOT_EYELET_VAL,KNOT_POT_VAL,1-KNOT_BUTTON_VAL,0);
  PrintToSerial(C,0,0,0);
  
  while(true){
    //Waiting to start
    KNOT_BUTTON_VAL = digitalRead(KNOT_BUTTON);
    KNOT_EYELET_VAL = digitalRead(KNOT_EYELET);
    KNOT_POT_VAL = analogRead(KNOT_POT);
    if(KNOT_BUTTON_VAL == 1){
      //Task has started
      Knot_Start = millis();
      digitalWrite(ERROR_LED,HIGH);
      break; 
    }
    serialReader();
    if (Serial_String_Complete){
      SWITCH_STATE = 1;
      return 2;
    }
    Knot_Time_Stamp = millis() - Knot_Time_Stamp_Start;
    PrintToFile(Knot_Time_Stamp,1,1-KNOT_EYELET_VAL,KNOT_POT_VAL,1-KNOT_BUTTON_VAL,0);
  }
  unsigned long bounce_time = millis();
  unsigned long bounce_prevent;
  if(KNOT_BUTTON_VAL == 1){
    //Participant has signal he has started the task
    tying_Progress = 1; 
  }
  
  //Wait for signal to be done
  int offalready = 0;
  bounce_prevent = millis() - bounce_time;
  while(true){
    bounce_prevent = millis() - bounce_time;
    //waiting for participant to signal he is done
    KNOT_BUTTON_VAL = digitalRead(KNOT_BUTTON);
    KNOT_EYELET_VAL = digitalRead(KNOT_EYELET);
    KNOT_POT_VAL = analogRead(KNOT_POT);
    if(KNOT_BUTTON_VAL == 1 && bounce_prevent >= 3000){
     Knot_End = millis() - Knot_Start;
     digitalWrite(ERROR_LED,HIGH);
     //Participant has signallled he is done
     break; 
    }
    serialReader();
    if (Serial_String_Complete){
      SWITCH_STATE = 1;
      return 2;
    }
    if(KNOT_EYELET_VAL == 0){
       digitalWrite(KNOT_LED,HIGH);
       Nearest_Pot_Point = (double) KNOT_POT_VAL;
    }
    else{
      digitalWrite(KNOT_LED,LOW);
    }
    
    Knot_Time_Stamp = millis() - Knot_Time_Stamp_Start;
    PrintToFile(Knot_Time_Stamp,2,1-KNOT_EYELET_VAL,KNOT_POT_VAL,1-KNOT_BUTTON_VAL,0);
    if(offalready == 0 && bounce_prevent >= 1000){
      digitalWrite(ERROR_LED,LOW);
      offalready = 1;
    }
    
  }
  
  
  tying_Progress = 2;
  Knot_Time_Stamp = millis() - Knot_Time_Stamp_Start;
  PrintToFile(Knot_Time_Stamp,3,1-KNOT_EYELET_VAL,KNOT_POT_VAL,1-KNOT_BUTTON_VAL,0);
  digitalWrite(KNOT_LED,LOW);

  delay(1000);
  digitalWrite(ERROR_LED,LOW);
  
  //Applying Force
  Motor_Start = millis();
  Knot_Time_Stamp = millis() - Knot_Time_Stamp_Start;
  PrintToFile(Knot_Time_Stamp,4,1-KNOT_EYELET_VAL,KNOT_POT_VAL,0,1); 
  for(pos = Slack_End; pos < Force_End; pos += 1) 
  {                                
    Servo_Motor.write(pos);
    delay(15);
  }
  while(true){
    //Holding force using weight
     KNOT_EYELET_VAL = digitalRead(KNOT_EYELET);
     KNOT_POT_VAL = analogRead(KNOT_POT);

     Motor_End = millis() - Motor_Start;
     if(Motor_End >= Motor_Time_Test){
       break; 
     }
     serialReader();
     if (Serial_String_Complete){
       SWITCH_STATE = 1;
       return 2;
     }
     if(KNOT_POT_VAL <= Breakoff_Pot_Point){
       KNOT_SNAP = 1;
       return 3;
     }
     if(KNOT_POT_VAL <= Furthest_Seperation){
       Furthest_Seperation = (double) KNOT_POT_VAL;
     }
     if(KNOT_EYELET_VAL == 0){
         digitalWrite(KNOT_LED,HIGH);
      }
      else{
        digitalWrite(KNOT_LED,LOW);
      }
      Knot_Time_Stamp = millis() - Knot_Time_Stamp_Start;
      PrintToFile(Knot_Time_Stamp,4,1-KNOT_EYELET_VAL,KNOT_POT_VAL,0,1); 
  }
  digitalWrite(KNOT_LED,LOW);
  delay(10);
    
  //Relieving Force
  //relieve Torque for Servo weight
  Knot_Time_Stamp = millis() - Knot_Time_Stamp_Start;
  PrintToFile(Knot_Time_Stamp,5,1-KNOT_EYELET_VAL,KNOT_POT_VAL,0,0); 
  for(pos = Force_End; pos >= Slack_End; pos -= 1) 
  {                                
    Servo_Motor.write(pos);
    delay(30);
  }

   //Print final data
   PrintToSerial(D,Furthest_Seperation,Nearest_Pot_Point,Knot_End);
   Knot_Time_Stamp = millis() - Knot_Time_Stamp_Start;
   PrintToFile(Knot_Time_Stamp,6,1-KNOT_EYELET_VAL,KNOT_POT_VAL,0,0); 
  
  SD_Print_File.close();
  return 1; 
}



void serialReader(){
  Serial_Return_Val = 0;
  Serial_String_Complete = false;
  int makeSerialStringPosition;
  int inByte;
  int length;
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
      else if(strcmp(serialReadString, "RESET") == 0){
        softreset();
      }
      else{
        if(!ID_Received){
          length = sizeof(serialReadString);
          if(length >= 4){
            ParticipantID = String(serialReadString);
            ID_Received = true;
            Serial.print("ID received: ");
            Serial.println(ParticipantID);
          }
        }
      }
    }
  }
  
}


void PrintToSerial(char* code, unsigned long input1, unsigned long input2, unsigned long input3)
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


void create_folder(){
  char Folder_Char[50];
  Folder_Name.concat(ParticipantID);
  Folder_Name.toCharArray(Folder_Char, 50); 
  Serial.println(Folder_Char);
  if(SD_Initialized && ID_Received){
   if(SD.mkdir(Folder_Char)){
     Folder_Created = true;
     Serial.println("Folder Created");
   }
  }
}

void create_file(int tasknum){
  char File_Char[70];
  String File_Name = Folder_Name;
  String taskstring = "/Task1.txt";
  if(tasknum == 1){
    if(Task1Try == 1){
      taskstring = "/Task101.txt";
    }
    else if(Task1Try == 2){
      taskstring = "/Task102.txt";
    }  
    else if(Task1Try == 3){
      taskstring = "/Task103.txt";
    } 
    else if(Task1Try == 4){
      taskstring = "/Task104.txt";
    } 
    else if(Task1Try == 5){
      taskstring = "/Task105.txt";
    } 
    else if(Task1Try == 6){
      taskstring = "/Task107.txt";
    } 
    else if(Task1Try == 7){
      taskstring = "/Task107.txt";
    }
    else{
      taskstring = "/Task100.txt";
    }
    Task1Try++;
  }
  else if(tasknum == 2){
   if(Task2Try == 1){
      taskstring = "/Task201.txt";
    }
    else if(Task2Try == 2){
      taskstring = "/Task202.txt";
    }  
    else if(Task2Try == 3){
      taskstring = "/Task203.txt";
    } 
    else if(Task2Try == 4){
      taskstring = "/Task204.txt";
    } 
    else if(Task2Try == 5){
      taskstring = "/Task205.txt";
    } 
    else if(Task2Try == 6){
      taskstring = "/Task207.txt";
    } 
    else if(Task2Try == 7){
      taskstring = "/Task207.txt";
    }
    else{
      taskstring = "/Task200.txt";
    } 
    Task2Try++;
  }
  else{
    taskstring = "/NoTask.txt";
  }
  
  File_Name.concat(taskstring);
  File_Name.toCharArray(File_Char, 70); 
  if(SD_Initialized && Folder_Created){
     SD_Print_File = SD.open(File_Char, FILE_WRITE);
     if(SD_Print_File){
       File_Created = true;
       if(tasknum == 1){
        MakeHeader(ParticipantID, "Ring Tower Transfer", "Time Stamp(s)", "Task Seq", "On Base #", "Wire Touch", "Total Touches", "Touch Time"); 
       }
       else if(tasknum == 2){
        MakeHeader(ParticipantID, "Knot Tying", "Time Stamp(s)", "Task Seq", "Eyelet Touch", "Pot Value", "Button Val", "Motor On"); 
       }
     }
  }
}

double timesec;
void PrintToFile(unsigned long time_stamp_mil, int taskseq, unsigned long input1, unsigned long input2, unsigned long input3, unsigned long input4){
  if(File_Created){
    timesec = (double) time_stamp_mil;
    timesec = floor(timesec*100.0) /100000.0;
    SD_Print_File.print(timesec,DEC);
    SD_Print_File.print("\t");
    SD_Print_File.print(taskseq,DEC);
    SD_Print_File.print("\t");
    SD_Print_File.print(input1,DEC);
    SD_Print_File.print("\t");
    SD_Print_File.print(input2,DEC);
    SD_Print_File.print("\t");
    SD_Print_File.print(input3,DEC);
    SD_Print_File.print("\t");
    SD_Print_File.print(input4,DEC);
    SD_Print_File.println();  // send a carriage return
  }
}

void MakeHeader(String ID, String TaskValue, String Column1, String Column2, String Column3, String Column4, String Column5, String Column6){
  if(File_Created){
    SD_Print_File.print("%Fundamentals of Robotic Surgery, Testee ID # ");
    SD_Print_File.print(ID);
    SD_Print_File.print(" Task: ");
    SD_Print_File.println(TaskValue);
    SD_Print_File.print("%");
    SD_Print_File.print(Column1);
    SD_Print_File.print("\t");
    SD_Print_File.print(Column2);
    SD_Print_File.print("\t");
    SD_Print_File.print(Column3);
    SD_Print_File.print("\t");
    SD_Print_File.print(Column4);
    SD_Print_File.print("\t");
    SD_Print_File.print(Column5);
    SD_Print_File.print("\t");
    SD_Print_File.print(Column6);
    SD_Print_File.println();  // send a carriage return
  }
}

void softreset(){
  digitalWrite(RESET_PIN, LOW); //Pulling the RESET pin LOW triggers the reset.
  
}


