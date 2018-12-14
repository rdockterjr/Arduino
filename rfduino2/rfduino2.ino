/*
This sketch demonstrates how to send data from a Device
to a Host in a Gazell network.

When Button A on the Device is pressed and released,
the green led on the host will toggle.
*/

#include <RFduinoGZLL.h>

device_t role = DEVICE0;

// pin for Button A on the RGB Shield
int ledpin = 1;

// starting state is off
int flashes = 0;

// starting state is off
int flashflag = 0;

void setup()
{
  pinMode(ledpin, OUTPUT);

  // start the GZLL stack
  RFduinoGZLL.begin(role);
}

void loop()
{
  while( flashflag == 0){
    //wait
    delay(30); 
  }


  for(int ii = 0; ii < flashes; ii++){
    digitalWrite(ledpin,HIGH);
    delay(30);
    digitalWrite(ledpin,LOW);
    delay(30);
  }
  flashflag = 0;

  
}

void RFduinoGZLL_onReceive(device_t device, int rssi, char *data, int len)
{
    flashes = 3; //atoi(data);
    flashflag = 1;
    RFduinoGZLL.sendToHost(data);
}
