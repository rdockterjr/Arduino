// connect ESP8266_Thing to internet
//Check button press for 

#include <ESP8266WiFi.h>
#include <ESP8266WiFiAP.h>
#include <ESP8266WiFiGeneric.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WiFiScan.h>
#include <ESP8266WiFiSTA.h>
#include <ESP8266WiFiType.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <WiFiServer.h>
#include <WiFiUdp.h>

WiFiClient client;

const char* myKey = "XDcJL9wqYNH1Tn02eBIIE";            // Your maker key goes here
const char* ssid = "Fort Boog";          // Your router's ssid goes here
const char* password = "L4cY1sAD0g!";  // Password to router goes here
bool canConnect = false;

//button stuff
long startTime = 0;
long startTimeReal = startTime;
int buttonPin = 4;
int bonoff = 0; //flip flop
long minuteThresh = 3; // two minutes open

void setup() {
  Serial.begin(115200); // Start serial communication for debug information
  Serial.println("Hello Computer ");
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(buttonPin,INPUT_PULLUP);

  // Setup the WiFi radio to connect to a wireless access point
  WiFi.mode(WIFI_STA); 
  ConnectWifi();        // Connect to WiFi
}

//check if button has been pressed for long enough to trigger notification
void buttonLogic(){
  
  //check if button is pressed
  int buttonval = digitalRead(buttonPin);
  if(buttonval == LOW){
    if(bonoff == 0){
      bonoff = 1;
      startTime = millis(); //for the timer
      startTimeReal = startTime; //for sending acutal duration
    }
  }
  else{
    //reset
    bonoff = 0;
    startTime = millis();
  }

  //see how long weve been pressed for
  if(bonoff == 1){
    long duration = (millis() - startTime) / 1000;
    Serial.println(duration);
    if(duration >= (minuteThresh * 60) ){
      //now lets send a post to iftt
      long durationreal = (millis() - startTimeReal) / 1000;
      post2IFTT(int(durationreal)); 
      //reset duration clock (so we only notify every n minutes)
      startTime = millis();
    }
  }
  
}

void loop()
{  
  // Make sure we're still connected to our router
  if(WiFi.status() == WL_CONNECTED ) 
  {
    if(!canConnect){
      //make sure we can connect to maker
      Serial.println("connecting...");
      if(client.connect("maker.ifttt.com",80))  // Test the connection to the server
      {
        canConnect = true;
        Serial.println("Connected to Maker");

      }
      else
      {
        delay(500);
        Serial.println("Failed to connect to Maker.");
      }
    }

    //check our button
    buttonLogic();

    //delay (since we are on the minute scale its okay if we have delay)
    delay(2000);

    //the yield lets stuff background on esp
    yield();
  }
  else  // Reconnect to wireless access point
  {
    delay(2000);    // Wait 1 second between connection attempts
    ConnectWifi();  // Connect to WiFi
  }
}



// Tries to connect to the wireless access point with the credentials provided
bool ConnectWifi() 
{
  bool timeOut = 0; // Change to 1 if connection times out
  byte count = 0;   // Counter for the number of attempts to connect to wireless AP
  
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password); // Connect to WiFi network

  while (WiFi.status() != WL_CONNECTED && (timeOut == 0)) // Test to see if we're connected
  {
    Serial.print('.');
    count++;
    
    if(count > 60) break; // Give up after ~30 seconds
    else delay(500);      // Check again after 500ms
  }
  
  if (WiFi.status() == WL_CONNECTED)  // We're connected
  {
    Serial.println("\r\nWiFi connected");
  }
  else  // Unable to connect
  {
    WiFi.disconnect();
    Serial.println("\r\nConnection Timed Out!\r\n");
  }
}



// Built HTTP packet to send to IFTT
void post2IFTT(int sendval) 
{
  //connec to iftt and send message
  if(client.connect("maker.ifttt.com",80)){
    String postData = "{\"value1\":\""; // JSON value to send
    postData.concat(sendval);
    postData.concat("\"}");
  
    //http posting packet
    client.print("POST /trigger/");
    client.print("GARAGE");
    client.print("/with/key/");
    client.print(myKey);
    client.println(" HTTP/1.1");
  
    //server is maker.iftt.com
    client.println("Host: maker.ifttt.com");
    client.println("User-Agent: ESP32_Thing");
    client.println("Connection: close");
  
    client.println("Content-Type: application/json");
    client.print("Content-Length: ");
    client.println(postData.length());
    client.println();
    client.println(postData);
  }

}


