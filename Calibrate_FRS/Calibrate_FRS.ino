//for motor
#include <Servo.h>
#include <SD.h>
#include <string.h>
#include <stdlib.h>

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
  
  int pos = 0;
  //relieve Torque for Servo weight
  for(pos = 90; pos >= 0; pos -= 1) 
  {                                
    Servo_Motor.write(pos);
    delay(30);
  }
 
  Serial.begin(9600);
  while(!Serial){
  }
  
  establishContact();
}

//TASK NUMBERS
//TASK 1: Ring tower transfer
//TASK 2: Knot Tying
//....

void loop()
{
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
     digitalWrite(RING_1_LED,HIGH);
     RING_1_BC_INIT = digitalRead(RING_1_BC);
     RING_2_RBC_INIT = digitalRead(RING_2_RBC);
     digitalWrite(RING_2_LED,HIGH);
     RING_2_BC_INIT = digitalRead(RING_2_BC);
//     RING_3_RBC_INIT = digitalRead(RING_3_RBC);
//     RING_3_BC_INIT = digitalRead(RING_3_BC);
//     digitalWrite(RING_3_LED,HIGH);
//     RING_4_RBC_INIT = digitalRead(RING_4_RBC);
//     RING_4_BC_INIT = digitalRead(RING_4_BC);
//     digitalWrite(RING_4_LED,HIGH);
     KNOT_BUTTON_INIT = digitalRead(KNOT_BUTTON);
     KNOT_EYELET_INIT = digitalRead(KNOT_EYELET);
     KNOT_POT_INIT = analogRead(KNOT_POT);
     digitalWrite(ERROR_LED,HIGH);
     digitalWrite(KNOT_LED,HIGH);
     
     if(RING_1_RBC_INIT == 1){
       Serial.println("error ring 1 base wires");
     } 
     if(RING_SC_INIT == 1){
       Serial.println("error Capacitance");
     }
     if(RING_1_BC_INIT == 1){
       Serial.println("error ring 1 base");
     }
     if(RING_2_RBC_INIT == 0){
       Serial.println("error ring 2 base wires");
     }
     if(RING_2_BC_INIT == 1){
        Serial.println("error ring 2 base ");
     }
//     if(RING_3_RBC_INIT == 1){
//       Serial.println("error ring 3 base wires");
//     }
//     if(RING_3_BC_INIT == 1){
//       Serial.println("error ring 3 base ");
//     }
//     if(RING_4_RBC_INIT == 0){
//      Serial.println("error ring 4 base wires ");
//     }
//     if(RING_4_BC_INIT == 1){
//       Serial.println("error ring 4 base ");
//     }
     
     if(KNOT_BUTTON_INIT == 1){
      Serial.println("error Knot Button");
     }
     if(KNOT_EYELET_INIT == 0){
      Serial.println("error Knot eyelets");
     }
   }
   int retpot = 0;
   retpot = analogRead(KNOT_POT_INIT);
   Serial.print("The Slide pot val is ");
   Serial.println(retpot);
   delay(4000);
   digitalWrite(RING_1_LED,LOW);
   digitalWrite(RING_2_LED,LOW);
//   digitalWrite(RING_3_LED,LOW);
//   digitalWrite(RING_4_LED,LOW);
   digitalWrite(ERROR_LED,LOW);
   digitalWrite(KNOT_LED,LOW);
}
