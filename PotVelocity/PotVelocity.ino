int analogPin = 2;     
                       
int val = 0;  
int pval = 0; 
float veloc = 0.0;
float veloc_rads = 0.0;
bool last_valid=false;


float pot2rads = 6.2832/1540;

unsigned long stime, dt;

void setup()
{
  Serial.begin(9600);          //  setup serial
  //digitalWrite(A2, HIGH);
  stime = millis()+1;
}

void loop()
{
  
  val = analogRead(analogPin);    // read the input pin
  bool in_valid_range=(val<1000 && val > 100);
  if(in_valid_range){
    dt = millis()-stime;
    
    veloc = float(val - pval)/float(dt)*1000;
    veloc_rads = veloc*pot2rads;
    stime = millis();
    if(last_valid){
      Serial.print(millis());
      Serial.print(", ");
      Serial.print(val);
      Serial.print(", ");
      Serial.print(veloc_rads);
      Serial.print(", ");
      Serial.println(veloc);
    }
  }
  delay(30);
  last_valid=in_valid_range;
  pval = val;
}
