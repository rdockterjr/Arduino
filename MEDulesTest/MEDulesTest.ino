#include <MEDules.h>


MEDules mydules;

bool return1, return2, return3, return4;
int retval;

void setup(){
  Serial.begin(9600);
  retval = 0;
  return1 = mydules.setupSerial();
  
  return2 = mydules.setupTask("Suturing",1);
  
  return3 = mydules.setupMetrics(3, "Time", "Approx", "Total Score");
}


void loop(){
  retval = mydules.serialRead();
  
  if(retval == 1){
    return4 = mydules.serialWrite(false, 50,75,100);
    
  }
  
  if(retval == 2){
    return4 = mydules.serialWrite(true, 99,60,99);
    
  }
  
  delay(500);
}


