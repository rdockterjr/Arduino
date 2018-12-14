//RFDUINO to RFDUINO communication

#include <RFduinoGZLL.h>

device_t role = HOST;
device_t target = DEVICE0;

void setup()
{
  // initialize serial:
  Serial.begin(9600);

  // start the GZLL stack  
  RFduinoGZLL.begin(role);
}

void loop()
{
  // if there's any serial available, read it:
  while (Serial.available() > 0) {

    // look for the next valid integer in the incoming serial stream:
    int flashes = Serial.parseInt(); 
    // do it again:
    int another = Serial.parseInt(); 

    // look for the newline. That's the end of your
    // sentence:
    if (Serial.read() == '\n') {

       flashes = constrain(flashes,0,255);

       char state[3];   //declaring character array
        String str;  //declaring string
        
        str=String(flashes); //converting integer into a string
        str.toCharArray(state,3); //passing the value of the string to the character array

      // print the three numbers in one string as hexadecimal:
      Serial.print("I Recieved: ");
      Serial.print(state);
      Serial.print(" and ");
      Serial.println(another);

      // send state to Host
      RFduinoGZLL.sendToDevice(target,state);
    }
  }
}

void RFduinoGZLL_onReceive(device_t device, int rssi, char *data, int len)
{
  char state = data[0];
  Serial.print("Device Said: ");
  Serial.println(state);
}
