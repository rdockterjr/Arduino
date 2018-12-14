int analogPin = 2;     // potentiometer wiper (middle terminal) connected to analog pin 3
                       // outside leads to ground and +5
int val = 0;           // variable to store the value read

int val2 = 0;
int val3 = 250;
int val4 = 500;

void setup()
{
  Serial.begin(9600);          //  setup serial
  digitalWrite(A2, HIGH);
}

void loop()
{
  val = analogRead(analogPin);    // read the input pin
  val2 = val2 + 1;
  val3 = val3 + 1;
  val4 = val4 + 1;
  
  Serial.println(val);
  //printData(val,val2,val3,val4);
  
  delay(30);
}

void printData(int val_1,int val_2,int val_3,int val_4){
  Serial.print('H');
  Serial.print(',');
  Serial.print(val_1);
  Serial.print(',');
  Serial.print(val_2);
  Serial.print(',');
  Serial.print(val_3);
  Serial.print(',');
  Serial.print(val_4);
  Serial.print(',');
  Serial.println('Q');
}
