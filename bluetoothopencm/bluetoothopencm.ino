void setup(){
  //Serial2 Serial initialize
  Serial2.begin(9600);  
  //SerialUSB.begin();
}

int parseSerialInt() {
    int numUnread = Serial2.available();
    int val = 0;
    char inputer[40];
    // numUnread > 0 means that there are some unread bytes waiting
    for (int inder = 0; inder < numUnread; inder++) {
        // Read a single byte out:
        char b = Serial2.read();
        inputer[inder] = b;
    }
    if(numUnread > 0){
      val = atoi(inputer);
      // And then print it back:
      return val;
    }
}

void loop() {
    int retval = parseSerialInt();
    if(retval > 0 ){
     Serial2.print("You Sent: ");
     Serial2.println(retval); 
    }
    delay(100);
}
