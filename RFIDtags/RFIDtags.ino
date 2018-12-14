#include <SoftwareSerial.h>

SoftwareSerial RFID(2, 3); // RX and TX
int data1 = 0;
bool ok = false;
int ledpin = 12;


#define USERS 3 //IF YOU ADD NEW USER, INCREMENT THIS VALUE
int alltags [USERS][14] = {
  {2,52,50,48,48,56,50,51,52,66,51,52,55,3}, //John
  {2,52,50,48,48,56,50,52,65,49,56,57,50,3}, //Trevor
  {2,52,50,48,48,55,57,53,69,56,56,69,68,3} //ROD
};
int newtag[14] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0}; // used for read comparisons


void setup()
{
  RFID.begin(9600); // start serial to RFID reader
  Serial.begin(9600); // start serial to PC 
  
  Serial.println("STARTING RFID");

  pinMode(ledpin,OUTPUT);
}


boolean comparetag(int aa[14], int bb[14])
{
  boolean ff = false;
  int fg = 0;
  for (int cc = 0 ; cc < 14 ; cc++)
  {
    if (aa[cc] == bb[cc])
    {
      fg++;
    }
  }
  if (fg == 14)
  {
    ff = true;
  }
  return ff;
}



void checkmytags() // compares each tag against the tag just read
{
  ok = false; // this variable helps decision-making,

  //Loop through all users and do a comparison
  for(int kk = 0; kk < USERS; kk++){
    if (comparetag(newtag, alltags[kk]))
    {
      ok = true;
      break;
    }
  }
}


void readTags()
{
  ok = false;
  if (RFID.available() > 0) 
  {
    // read tag numbers
    delay(100); // needed to allow time for the data to come in from the serial buffer.
    for (int z = 0 ; z < 14 ; z++) // read the rest of the tag
    {
      data1 = RFID.read();
      newtag[z] = data1;
    }
    RFID.flush(); // stops multiple reads
    // do the tags match up?
    checkmytags();
  }
  
  // now do something based on tag type
  if (ok) // if we had a match
  {
    Serial.println("Accepted");
    digitalWrite(ledpin,HIGH);
    delay(2000);
    ok = false;

  }
  else if (!ok) // if we didn't have a match
  {
    //do something else
    digitalWrite(ledpin,LOW);
  }
}


void loop()
{
  readTags();
}

