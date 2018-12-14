

void setup()
{
  
  Serial.begin(9600);

}

void loop()
{
  //digitalWrite(2,HIGH);
  delay(1000);
  PrintToSerial("B", 98765, 12345);
  delay(1000);
  PrintToSerial("Y", 00004, 22222);
}

void PrintToSerial(char* code, unsigned long input1, unsigned long input2)
{

  Serial.print('H'); // unique header to identify start of message
  Serial.print(",");
  Serial.print(code);
  Serial.print(",");
  Serial.print(input1,DEC);
  Serial.print(",");
  Serial.print(input2,DEC);
  Serial.print(",");
  Serial.print("Q");  // note Q is sent to mark end of line
  Serial.println();  // send a cr/lf
}
