int ruptPin = 2; // select the input pin for the interrupter
int val = 0; // variable to store the value coming from the sensor

void setup()
{
  //pinMode(ruptPin, INPUT); 
    Serial.begin(9600); // set up Serial library at 9600 bps
}

void loop()
{
    val = analogRead(ruptPin); // read the value from the sensor
    Serial.println(val); // print the sensor value to the serial monitor
    delay(50);
    
}
