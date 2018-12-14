/*
 Serial2 : TX = A4, RX = A5
*/

int value = 100;

void setup(){
  //Serial2 Serial initialize
  Serial2.begin(9600);  
}
void loop(){
  // when you typed any character in terminal
  if(Serial2.available()){
     Serial2.println("You sent Something");
  }
    //print it out though USART2(RX2,TX2)
    Serial2.print("Num= ");
    //Serial2.println((char)Serial2.read());
    Serial2.println(value);
    value = value + 2;
    delay(2000);
}

