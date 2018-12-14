
#define BUFFER_SIZE 1024

//setup connection params
String SSID  = "Fort Boog";     // change this to match your WiFi SSID
String PASS  = "L4cY1sAD0g!"; // change this to match your WiFi password

String host     = "maker.ifttt.com";
const int httpPort   = 80;
String key = "XDcJL9wqYNH1Tn02eBIIE"; // Your IFTTT Maker key
String event = "ESP"; // Your IFTTT event name
String uri     = "/trigger/" + event + "/with/key/" + key;

char buffer[BUFFER_SIZE];


int errorLED = 13;

// By default we are looking for OK\r\n
char OKrn[] = "OK\r\n";
byte wait_for_esp_response(int timeout, char* term = OKrn) {
  unsigned long t = millis();
  bool found = false;
  int i = 0;
  int len = strlen(term);
  // wait for at most timeout milliseconds
  // or if OK\r\n is found
  while (millis() < t + timeout) {
    if (Serial1.available()) {
      buffer[i++] = Serial1.read();
      if (i >= len) {
        if (strncmp(buffer + i - len, term, len) == 0) {
          found = true;
          break;
        }
      }
    }
  }
  buffer[i] = 0;
  Serial.print(buffer);
  return found;
}

void setup() {

  // assume esp8266 operates at 115200 baud rate
  // change if necessary to match your modules' baud rate
  Serial1.begin(115200);  // Teensy Hardware Serial port 1   (pins 0 and 1)
  Serial.begin(115200);   // Teensy USB Serial Port

  pinMode(errorLED,OUTPUT);
  digitalWrite(errorLED,LOW);

  delay(5000);
  Serial.println("begin.");


  //reset and connect
  resetWifi();
  connectWifi();
  //waitforit
  while(1){
    if (Serial1.find("WIFI GOT IP")) {
      break;
    }
  }
  checkIP();
  SetupConnection();

  
  //now lets send it
  int val = analogRead(20);
  httppost(val);
}

//reset the esp8266 module
void resetWifi() {
  Serial1.println("AT+RST");
  wait_for_esp_response(1000);
}

//connect to wifi
void connectWifi() {
  String cmd = "AT+CWJAP=\"" + SSID + "\",\"" + PASS + "\"";
  Serial1.println(cmd);
  wait_for_esp_response(5000);
}

//Returns our current IP address
void checkIP(){
  // print device IP address
  Serial1.println("AT+CIFSR");
  wait_for_esp_response(2000);
}

//need this to use CIPSTART
void SetupConnection(){
  Serial1.println("AT+CIPMUX=0");
  wait_for_esp_response(1000);
  Serial1.println("AT+CWMODE=1");
  wait_for_esp_response(1000);
}

//open TCP connection ifttt
void OpenConnection(){
  Serial1.println("AT+CIPSTART=\"TCP\",\"" + host + "\"," + httpPort);
  wait_for_esp_response(1000);
}

//post to http site
void httppost(int dataval) {
  //open connection
  OpenConnection();

  //compile our json packet data
  String postData = "{\"value1\":\"";
  postData.concat(dataval);
  postData.concat("\"}");
  postData.concat("\r\n");
  int contentlength = postData.length();
  
  // Construct our HTTP call
  String httpPacket = "POST " + uri + " HTTP/1.1\r\nHost: " + host + "\r\nContent-Length: " + contentlength + " \r\nContent-Type: application/json\r\n\r\n" + postData +"\r\n";
  int packetlength = httpPacket.length();
  
 // Send our message length
  Serial1.print("AT+CIPSEND=");
  Serial1.println(packetlength);
  wait_for_esp_response(1000);
  if (!Serial1.find(">")) digitalWrite(errorLED, HIGH);

  // Send our http request
  Serial1.print(httpPacket);
  delay(10); 
  if (!Serial1.find("SEND OK\r\n")) digitalWrite(errorLED, HIGH);
  
  delay(1000);
}


void loop() {
  // Send bytes from Computer <-> Teensy back to ESP8266
  if ( Serial.available() ) {
    Serial1.write( Serial.read() );
  }
  if ( Serial1.available() ) {
      Serial.write( Serial1.read() );
  }
  digitalWrite(errorLED, LOW);
}


