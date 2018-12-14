
#define DXL_BUS_SERIAL1 1  //Dynamixel on Serial1(USART1)  <-OpenCM9.04

#define LEFT_WHEEL 3
#define RIGHT_WHEEL 4

int midspeed = 150;
int speedmult = 20;
int speedVal = 0;

Dynamixel Dxl(DXL_BUS_SERIAL1);

void setup() {
  
  //serial
  Serial2.begin(9600); 
  
  // Initialize the dynamixel bus:

  // Dynamixel 2.0 Baudrate -> 0: 9600, 1: 57600, 2: 115200, 3: 1Mbps 
  Dxl.begin(3);
  Dxl.wheelMode(LEFT_WHEEL); //wheelMode() is to use wheel mode
  Dxl.wheelMode(RIGHT_WHEEL); //wheelMode() is to use wheel mode
  
  //stop
  Dxl.goalSpeed(LEFT_WHEEL, 0);
  Dxl.goalSpeed(RIGHT_WHEEL, 0);
}

void wheelControl(int val){
  if(val <= 10){
    speedVal = val*speedmult;
    //both forward
    Dxl.goalSpeed(LEFT_WHEEL, speedVal);
    Dxl.goalSpeed(RIGHT_WHEEL, speedVal | 0x400);
  }
  else if(val <= 20){
    speedVal = (val-10)*speedmult;
    //both reverse
    Dxl.goalSpeed(LEFT_WHEEL, speedVal | 0x400);
    Dxl.goalSpeed(RIGHT_WHEEL, speedVal);
  }
  else if(val <= 30){
    speedVal = (val-20)*speedmult;
    //turn right
    Dxl.goalSpeed(LEFT_WHEEL, speedVal);
    Dxl.goalSpeed(RIGHT_WHEEL, speedVal);
  } 
  else if(val <= 40){
    speedVal = (val-30)*speedmult;
    //turn left
    Dxl.goalSpeed(LEFT_WHEEL, speedVal | 0x400);
    Dxl.goalSpeed(RIGHT_WHEEL, speedVal | 0x400);
  }
  else if(val <= 50){
    //stop
    Dxl.goalSpeed(LEFT_WHEEL, 0);
    Dxl.goalSpeed(RIGHT_WHEEL, 0);
  } 
  
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
     wheelControl(retval);
     Serial2.println(retval); 
    }
    delay(100);
}
