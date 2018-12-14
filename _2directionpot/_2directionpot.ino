int analogPin0 = 0; 
int analogPin1 = 1; 
int buttonPin = 2;

int val0 = 0;
int val1 = 0;
int buttonPressed = 0;

void setup()
{
  
  pinMode(buttonPin, INPUT_PULLUP); 
  
  Serial.begin(9600);          //  setup serial
}



void loop()
{
  buttonPressed = digitalRead(buttonPin);
  val0 = analogRead(analogPin0); 
  val1 = analogRead(analogPin1); 
  
  Serial.print(val0);        
  Serial.print(",");
  Serial.print(val1);   
  Serial.print(",");
  Serial.println(buttonPressed);   
  
  delay(30);
}
