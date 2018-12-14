//STYLE 1:
//PIN 1.   GROUND
//2.   + OUTPUT
//3.   + SUPPLY
//4.   - OUTPUT

double mv2ard = 5000.0/512;
double kpa2mv = 1.0/2.5;
int outputplus = 0;
int outputminus = 1;
int buttonid = 3;
int readplus;
int readminus;
double rawdiff;
double kpadiff;
unsigned long programstart;
unsigned long mseconds;
double seconds;
int buttonval = 0;

void setup() {
  pinMode(buttonid,INPUT);
  Serial.begin(9600);
  Serial.println("Ready");
  

}

void loop() {
    int kpamax = 50;
    programstart = millis();
    
    readplus = analogRead(outputplus);
    readminus = analogRead(outputminus);
    rawdiff = double(readminus) - double(readplus);
    kpadiff = rawdiff*mv2ard*kpa2mv;
    buttonval = digitalRead(buttonid);
    if(kpadiff >50 && buttonval == 1){
      delay(200);
      while(true){
        readplus = analogRead(outputplus);
        readminus = analogRead(outputminus);
        rawdiff = double(readminus) - double(readplus);
        kpadiff = rawdiff*mv2ard*kpa2mv;
        buttonval = digitalRead(buttonid);
        if(kpadiff > kpamax){
         kpamax = kpadiff; 
        }
        if(kpadiff <= 2 || buttonval == 1){
          mseconds = millis() - programstart;
          seconds = double(mseconds) / 1000.0;
          break; 
        }
      }
      int kpadrop = kpamax - kpadiff;
      Serial.print("The pressure drop of ");
      Serial.println(kpadrop);
      Serial.print("Took time(s) of ");
      Serial.println(seconds);
    }
} 

