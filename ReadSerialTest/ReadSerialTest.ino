
int inbyte;
boolean Serial_String_Complete = false;  // whether the string is complete
int Serial_Return_Val = 0;

char A[] = "A";//Ring on base 1
char B[] = "B";//Ring on S 1
char C[] = "C";//Ring on second base / done
char D[] = "D";//Ring on second base / done

void setup(){
  pinMode(2,OUTPUT);
  Serial.begin(9600);
}

void loop(){

  serialReader();
  delay(2000);
  if (Serial_String_Complete) {
    if(Serial_Return_Val == 1){
      delay(500);
      printCombo(A,13213,1231,12313);
    }
    else if(Serial_Return_Val == 2){
      delay(500);
      printCombo(B,232,2333,24546);
    }
    else if(Serial_Return_Val == 9){
      delay(500);
      printCombo(C,0,0,0);
    }
    else{
    }
    delay(5000);
  }
  else{
    printCombo(D,12,23,34);
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


