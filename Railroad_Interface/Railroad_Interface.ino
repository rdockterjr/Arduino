//for motor
#include <string.h>
#include <stdlib.h>

//declare pins
int RESET_PIN = 13;
int BUTTON_PIN = 3; //for start / stop
int START_LED = 7; //in progress
int STOP_LED = 8; //done
int ANALOG_PRESSURE_1 = 0; //pressure gage pos 
int ANALOG_PRESSURE_2 = 1; //pressure gage neg
int PUMP_ON = 12; //pressure pump
int PRESSURE_RELIEF = 11; //pressure pump
unsigned long Pump_On_Limit = 5000;
unsigned long Deflate_Limit = 11000;
unsigned long Pressure_Val_Limit = 9;

//declare pin read values
int ANALOG_PRESSURE_1_VAL = 0; //pressue gage
int ANALOG_PRESSURE_2_VAL = 0; //pressue gage
int BUTTON_PIN_VAL = 0;
int Diff_Pressure = 0;

//Initial Pins
int ANALOG_PRESSURE_1_INIT = 0; //pressue gage
int ANALOG_PRESSURE_2_INIT = 0; //pressue gage
int BUTTON_PIN_INIT = 0;
int Diff_Init = 0;

//declare states
int SWITCH_STATE = 0;

//Serial Reader
int Serial_Return_Val = 0;
boolean Serial_String_Complete = false;
int inbyte;
unsigned long serialdata;

int initError = 0;

//timing variables
unsigned long Task_Start = 0;
unsigned long Task_End = 0;
unsigned long Pressure_Start = 0;
unsigned long Pressure_End = 0;
unsigned long Pump_Start = 0;
unsigned long Pump_End = 0;
unsigned long Deflate_Start = 0;
unsigned long Deflate_End = 0;
int Pressure_Max = 0;
int Pressure_Min = 5;

//Characters
//H and Q are protected
char A[] = "A";//Ring Task Started
char B[] = "B";//Ring Task Finished
char C[] = "C";//Early Exit

void setup()
{
  //setup pins
  digitalWrite(RESET_PIN, HIGH);
  pinMode(RESET_PIN,OUTPUT);
  pinMode(BUTTON_PIN,INPUT_PULLUP); //for start / stop
  pinMode(START_LED,OUTPUT); //in progress
  pinMode(STOP_LED,OUTPUT); //done
  //ANALOG_PRESSURE is analogRead; //pressure gage
  pinMode(PUMP_ON,OUTPUT); //pressure pump
  pinMode(PRESSURE_RELIEF,OUTPUT); //pressure pump
 
  Serial.begin(9600);
  while(!Serial){
  }
  serialReader();
  
  establishContact();
  
  delay(2000);
  Serial.flush();
  
  digitalWrite(PRESSURE_RELIEF,LOW);
}
void loop()
{

  serialReader();
  if (Serial_String_Complete){
    SWITCH_STATE = 0;
    if(Serial_Return_Val == 1){
      //Railroad
      int ret1;
      ret1 = performTask();
      if(ret1 == 1){
        SWITCH_STATE = 0;
        Serial_String_Complete = false;
        Serial_Return_Val = 0;
      }
      if(ret1 == 2){
        SWITCH_STATE = 1;
      }
    }
  }
  
  if(SWITCH_STATE == 1){
    SWITCH_STATE = 0;
    PrintToSerial(C,0,0,0,0);
  }
  
  delay(100); 
}


void establishContact() {
   int initerror = 0;
   if(true){
     ANALOG_PRESSURE_1_INIT = analogRead(ANALOG_PRESSURE_1);
     ANALOG_PRESSURE_2_INIT = analogRead(ANALOG_PRESSURE_2);
     Diff_Init = abs(ANALOG_PRESSURE_1_INIT-ANALOG_PRESSURE_2_INIT);
     BUTTON_PIN_INIT = digitalRead(BUTTON_PIN_VAL);
     
     if(Diff_Init >= 2){
       initerror = 1;
       //pressure in cavity
     } 
     if(BUTTON_PIN_INIT == 0){
       initerror = 2;
       //button stuck
     }

   }
   int yy = 0;
   if(initerror == 1){
     for(yy = 0; yy < 1; yy++){
       delay(500);
       digitalWrite(START_LED,HIGH);
       digitalWrite(STOP_LED,HIGH);
       delay(500);
       digitalWrite(START_LED,LOW);
       digitalWrite(STOP_LED,LOW);
     }
     delay(2000);
  }
  if(initerror == 2){
     for(yy = 0; yy < 2; yy++){
       delay(500);
       digitalWrite(START_LED,HIGH);
       digitalWrite(STOP_LED,HIGH);
       delay(500);
       digitalWrite(START_LED,LOW);
       digitalWrite(STOP_LED,LOW);
     }
  }
  delay(1000);
}


//////////////////////////////////

int performTask(){
  //Initialize
  SWITCH_STATE = 0;
  Task_End = 0;
  Pressure_Start = 0;
  Pressure_End = 0;
  
  //baseline pressure
  ANALOG_PRESSURE_1_VAL = analogRead(ANALOG_PRESSURE_1);
  ANALOG_PRESSURE_2_VAL = analogRead(ANALOG_PRESSURE_2);
  Diff_Pressure = abs(ANALOG_PRESSURE_1_VAL-ANALOG_PRESSURE_2_VAL);
  BUTTON_PIN_VAL = digitalRead(BUTTON_PIN);
  
  //set lights to aknowledge
  digitalWrite(START_LED,LOW);
  digitalWrite(STOP_LED,LOW);
  digitalWrite(START_LED,HIGH);
  delay(300);
  digitalWrite(START_LED,LOW);
  delay(200);
  digitalWrite(STOP_LED,HIGH);
  delay(300);
  digitalWrite(STOP_LED,LOW);
  digitalWrite(PUMP_ON,LOW);

  PrintToSerial(A,0,0,0,0);
  
  while(true){
    //Waiting to start
    BUTTON_PIN_VAL = digitalRead(BUTTON_PIN);
    if(BUTTON_PIN_VAL == 0){
      //Task has started
      Task_Start = millis();
      digitalWrite(START_LED,HIGH);
      break; 
    }
    serialReader();
    if (Serial_String_Complete){
      SWITCH_STATE = 1;
      return 2;
    }
  }
  unsigned long bounce_time = millis();
  unsigned long bounce_prevent;
  
  //Wait for signal to be done
  while(true){
    bounce_prevent = millis() - bounce_time;
    
    //waiting for participant to signal he is done
    ANALOG_PRESSURE_1_VAL = analogRead(ANALOG_PRESSURE_1);
    ANALOG_PRESSURE_2_VAL = analogRead(ANALOG_PRESSURE_2);
    Diff_Pressure = abs(ANALOG_PRESSURE_1_VAL-ANALOG_PRESSURE_2_VAL);
    BUTTON_PIN_VAL = digitalRead(BUTTON_PIN);
    
    if(BUTTON_PIN_VAL == 0 && bounce_prevent >= 3000){
     Task_End = millis() - Task_Start;
     digitalWrite(STOP_LED,HIGH);
     digitalWrite(START_LED,LOW);
     //Participant has signallled he is done
     break; 
    }
    serialReader();
    if (Serial_String_Complete){
      SWITCH_STATE = 1;
      return 2;
    }
    
  }

  delay(1000);
  digitalWrite(START_LED,HIGH);
  
  //Applying Force (Turn pump on)
  digitalWrite(PUMP_ON,HIGH);
  Pump_Start = millis();
  int blinktimer = 300;
  int on_off = 0;
  
  while(true){    
    ANALOG_PRESSURE_1_VAL = analogRead(ANALOG_PRESSURE_1);
    ANALOG_PRESSURE_2_VAL = analogRead(ANALOG_PRESSURE_2);
    Diff_Pressure = abs(ANALOG_PRESSURE_1_VAL-ANALOG_PRESSURE_2_VAL);
    
    //Check if pump has been on long enough
    Pump_End = millis() - Pump_Start;
    if(Pump_End >= Pump_On_Limit || Diff_Pressure >= Pressure_Val_Limit){
      digitalWrite(PUMP_ON,LOW);
      digitalWrite(START_LED,LOW);
      Pressure_Start = millis();
      break; 
    }
    
    //Check if task was canceled
    serialReader();
    if (Serial_String_Complete){
      SWITCH_STATE = 1;
      return 2;
    }
    
    //Check to see when max pressure is reached
    if(Diff_Pressure > Pressure_Max){
      Pressure_Max = Diff_Pressure;
    }
    
    //Flashing lights
    if(Pump_End >= blinktimer){
      blinktimer += 300;
      if(on_off == 0){
        digitalWrite(START_LED,LOW);
        on_off = 1;
      }
      else{
        digitalWrite(START_LED,HIGH);
        on_off = 0;
      }
    }
    
  }

  blinktimer = 300;
  on_off = 0;
  //Now wait for deflate
  Deflate_Start = millis();
  while(true){
     Deflate_End = millis() - Deflate_Start;
     ANALOG_PRESSURE_1_VAL = analogRead(ANALOG_PRESSURE_1);
     ANALOG_PRESSURE_2_VAL = analogRead(ANALOG_PRESSURE_2);
     Diff_Pressure = abs(ANALOG_PRESSURE_1_VAL-ANALOG_PRESSURE_2_VAL);
     
     if(Deflate_End >= Deflate_Limit || Diff_Pressure < 1){
       Pressure_End = Pressure_Start - millis();
       Pressure_Min = Diff_Pressure;
       digitalWrite(STOP_LED,LOW);
       break; 
     }
     
     //Check for pressure min
     if(Diff_Pressure < Pressure_Min){
       Pressure_Min = Diff_Pressure; 
     }
     
     //Flashing lights
    if(Deflate_End >= blinktimer){
      blinktimer += 300;
      if(on_off == 0){
        digitalWrite(STOP_LED,LOW);
        on_off = 1;
      }
      else{
        digitalWrite(STOP_LED,HIGH);
        on_off = 0;
      }
    }
  }

   //Print final data
   PrintToSerial(B,Task_End,Pressure_End,Pressure_Max,Pressure_Min);
   
   digitalWrite(PRESSURE_RELIEF,HIGH);
   //set lights to aknowledge
  digitalWrite(START_LED,LOW);
  digitalWrite(STOP_LED,LOW);
  digitalWrite(START_LED,HIGH);
  delay(300);
  digitalWrite(START_LED,LOW);
  delay(200);
  digitalWrite(STOP_LED,HIGH);
  delay(300);
  digitalWrite(STOP_LED,LOW);
  
  delay(200);
  digitalWrite(PRESSURE_RELIEF,LOW);

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
      if (strcmp(serialReadString, "TASK") == 0){
        Serial_Return_Val = 1;
      }
      else if(strcmp(serialReadString, "STOP") == 0){
        Serial_Return_Val = 9;
      }
      else if(strcmp(serialReadString, "RESET") == 0){
        softreset();
      }
    }
  }
  
}


void PrintToSerial(char* code, unsigned long input1, unsigned long input2, int input3, int input4)
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
  Serial.print(input4,DEC);
  Serial.print(",");
  Serial.print("Q");  // note Q is sent to mark end of line
  Serial.println();  // send a cr/lf
}


void softreset(){
  digitalWrite(RESET_PIN, LOW); //Pulling the RESET pin LOW triggers the reset.
  
}


