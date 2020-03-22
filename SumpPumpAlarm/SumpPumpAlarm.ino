//Wifi includes
#include <WiFiNINA.h>
#include <RTCZero.h>
#include "arduino_secrets.h" 

//defines
#define FLOATSWITCH_PIN 5
#define LED_PIN 13
#define DEBOUNCE_COUNT 10

//Wi-Fi connection
char ssid[] = SECRET_SSID;        // network SSID 
char pass[] = SECRET_PASS;    // network password
int wifi_status = WL_IDLE_STATUS;     // the WiFi radio's status

//IFTTT Parameters
char MakerIFTTT_Key[] = IFTTT_KEY; // key
char MakerIFTTT_Event[] = IFTTT_EVENT; // event
char MakerIFTTT_Server[] = IFTTT_SERVER; // server


// RTC Object
RTCZero rtc;
uint8_t day = 22;
uint8_t month = 02;
uint8_t year = 20;
uint8_t seconds = 00;
uint8_t minutes = 00;
uint8_t hours = 12;
uint8_t increment = 15; // every 15 minutes

//values
bool alarm_state, last_alarm_state, send_request;
unsigned int alarm_count;

void setup() {
  //initialize states
  alarm_state = false;
  last_alarm_state = false;
  send_request = false;
  alarm_count = 0;

  // float switch
  pinMode(FLOATSWITCH_PIN, INPUT_PULLUP);

  //LED
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  
  // check for the WiFi module:
  wifi_status = WiFi.status();
  if (wifi_status == WL_NO_MODULE) {
    // don't continue
    while (true);
  }

  //Setup RTC
  rtc.begin();
  rtc.setTime(hours, minutes, seconds);
  rtc.setAlarmTime(0, minutes, 0);    //Every Five Minutes
  rtc.enableAlarm(rtc.MATCH_MMSS);
  rtc.attachInterrupt(IntervalCallback);
}

void IntervalCallback() {
  send_request = true;
}

void loop() {
  //run this every interval
  if(send_request){
    send_request = false;
    WifiConnect();
    alarm_state = ReadFloatSwitch();
    SetPhysicalAlert();
    SendWifiAlert();
    WifiDisconnect();
    UpdateAlarmTime();
  }
}

//Read switch
bool ReadFloatSwitch(){
  // Read float switch
  int floatswitch_val = 0;
  for(int i = 0; i < DEBOUNCE_COUNT; i++){
    floatswitch_val += digitalRead(FLOATSWITCH_PIN);
    delay(100);
  }
  return (floatswitch_val == DEBOUNCE_COUNT);
}

//LEDS and Buzzers?
void SetPhysicalAlert(){
  if(alarm_state){
    digitalWrite(LED_PIN, HIGH);
  }
  else{
    digitalWrite(LED_PIN, LOW);
  }
}

//Where we send alarms
void SendWifiAlert(){
  if(alarm_state){
    bool post_success = ServerPost(true, alarm_count);
    alarm_count++;
  }
  else if(!alarm_state && last_alarm_state){
    bool post_success = ServerPost(false, alarm_count);
    alarm_count = 0;
  }

  last_alarm_state = alarm_state;
}

//flash led n times
void Flash(int pin, int times){
  for(int i = 0; i < times; i++){
    digitalWrite(pin, HIGH);
    delay(300);
    digitalWrite(pin, LOW);
    delay(300);
  }
}

//Send to server
bool ServerPost(bool level_high, unsigned int count) {
  if(wifi_status != WL_CONNECTED){
    return false;
  }
  WiFiSSLClient client;
  
  if(client.connect(MakerIFTTT_Server, 443)) {
    //json data
    String v1 = "Normal";
    if(level_high){
      v1 = "HIGH";
    }
    String v2 =  String(count, DEC);  
    //{"value1":"Low","value2":"1"}
    String ifttt_post_data =  String("value1=") + v1 + String("&value2=") + v2;
    String ifttt_post_data_size = String(ifttt_post_data.length());

    //request
    client.print("POST /trigger/");
    client.print(MakerIFTTT_Event);
    client.print("/with/key/");
    client.print(MakerIFTTT_Key);
    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.println(MakerIFTTT_Server);
    client.println("Connection: close");
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.print("Content-Length: ");
    client.println(ifttt_post_data_size);
    client.println();
    client.println(ifttt_post_data);
  } else {
    client.stop();
    return false;
  }

  // Listen and manage a timeout
  unsigned long startTime = millis();
  bool received = false;

  //check response
  while ((millis() - startTime < 500) && !received) {
    while(client.available()) {
      received = true;
      char c = client.read();
    }
  }
  client.stop();
  return true;
}

void WifiConnect() {
  // Attempt to connect (5 retry max)
  int status = WL_IDLE_STATUS;
  for(uint8_t retry = 0; retry < 5 && status != WL_CONNECTED; retry++) {
    // attempt to connect to WiFi network:
    Flash(LED_PIN, 5);
    
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);
  
    // wait 5 second for connection:
    delay(5000);
  }
  
  //alert user wifi broken
  if(status != WL_CONNECTED){
    digitalWrite(LED_PIN, HIGH);
  }

  wifi_status = status;
}

//Switch Off Wifi To Save Power
void WifiDisconnect() {
  WiFi.end();
  delay(1000);
  wifi_status = WiFi.status();
}

//increment by 15
void UpdateAlarmTime(){
  minutes = (minutes + increment) % 60;
  rtc.setAlarmTime(00,minutes,00);
  rtc.enableAlarm(rtc.MATCH_MMSS);
}

