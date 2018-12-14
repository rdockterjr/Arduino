#include <SoftwareSerial.h>

SoftwareSerial mySerial(2, 3); // RX, TX

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  
  Serial.println("YOLO");

  // set the data rate for the SoftwareSerial port
  mySerial.begin(9600);
}

void loop() { // run over and over
  if(mySerial.available()){
    Serial.println("packet");
    for (int cc = 0 ; cc < 14 ; cc++){
      Serial.print(mySerial.read());
      if(cc < 13){
        Serial.print(",");
      }
    }
    Serial.println(" ");
  }
  delay(1000);
}
