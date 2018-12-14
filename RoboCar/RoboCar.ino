// Car control



#include <AFMotor.h>
#include <SoftwareSerial.h>

//bumpers
#define frontbumper A0
#define backbumper A1

int frontbumpint = 1;
int backbumpint = 1;

bool movinforward = false;
bool movinbackward = false;


//motor objects
AF_DCMotor motor_lf(1);
AF_DCMotor motor_rf(4);
AF_DCMotor motor_lb(2);
AF_DCMotor motor_rb(3);

//can change
int speedAll = 240;
int halfSpeed = 150;
char serialIn;

//serial
SoftwareSerial mySerial(9, 10); // RX(servo1), TX(servo2)

void setup() {
  Serial.begin(9600);           // set up Serial library at 9600 bps
  Serial.println("RoboCar!");

  pinMode(frontbumper,INPUT_PULLUP);
  pinMode(backbumper,INPUT_PULLUP);

  mySerial.begin(9600);
   
  // turn on motors
  motor_lf.setSpeed(0);
  motor_lf.run(RELEASE);
  motor_rf.setSpeed(0);
  motor_rf.run(RELEASE);
  motor_lb.setSpeed(0);
  motor_lb.run(RELEASE);
  motor_rb.setSpeed(0);
  motor_rb.run(RELEASE);
}


//Control all motor speeds
void setMotorSpeeds(int lf, int rf, int lb, int rb){
  //left front
  if(lf < 0){
    motor_lf.setSpeed(abs(lf));
    motor_lf.run(FORWARD);
  }
  else if(lf == 0){
    motor_lf.setSpeed(abs(lf));
    motor_lf.run(RELEASE);
  }
  else{
    motor_lf.setSpeed(abs(lf));
    motor_lf.run(BACKWARD);
  }
  //right front
  if(rf < 0){
    motor_rf.setSpeed(abs(rf));
    motor_rf.run(FORWARD);
  }
  else if(rf == 0){
    motor_rf.setSpeed(abs(rf));
    motor_rf.run(RELEASE);
  }
  else{
    motor_rf.setSpeed(abs(rf));
    motor_rf.run(BACKWARD);
  }
  //left back
  if(lb < 0){
    motor_lb.setSpeed(abs(lb));
    motor_lb.run(FORWARD);
  }
  else if(lb == 0){
    motor_lb.setSpeed(abs(lb));
    motor_lb.run(RELEASE);
  }
  else{
    motor_lb.setSpeed(abs(lb));
    motor_lb.run(BACKWARD);
  }
  //right back
  if(rb < 0){
    motor_rb.setSpeed(abs(rb));
    motor_rb.run(FORWARD);
  }
  else if(rb == 0){
    motor_rb.setSpeed(abs(rb));
    motor_rb.run(RELEASE);
  }
  else{
    motor_rb.setSpeed(abs(rb));
    motor_rb.run(BACKWARD);
  }
}

//For using wasd
void inputControl(char inp){
  //for keyboard control
  if(inp == 'x'){
    setMotorSpeeds(0,0,0,0);
    movinforward = false;
    movinbackward = false;
  }
  else if(inp == 'w'){
    setMotorSpeeds(speedAll,speedAll,speedAll,speedAll);
    movinforward = true;
    movinbackward = false;
  }
  else if(inp == 's'){
    setMotorSpeeds(-speedAll,-speedAll,-speedAll,-speedAll);
    movinforward = false;
    movinbackward = true;
  }
  else if(inp == 'a'){
    setMotorSpeeds(0,speedAll,0,speedAll);
    movinforward = true;
    movinbackward = false;
  }
  else if(inp == 'd'){
    setMotorSpeeds(speedAll,0,speedAll,0);
    movinforward = true;
    movinbackward = false;
  }
  else if(inp == 'q'){
    setMotorSpeeds(halfSpeed,speedAll,halfSpeed,speedAll);
    movinforward = true;
    movinbackward = false;
  }
  else if(inp == 'e'){
    setMotorSpeeds(speedAll,halfSpeed,speedAll,halfSpeed);
    movinforward = true;
    movinbackward = false;
  }
  else if(inp == 'z'){
    setMotorSpeeds(-halfSpeed,-speedAll,-halfSpeed,-speedAll);
    movinforward = false;
    movinbackward = true;
  }
  else if(inp == 'c'){
    setMotorSpeeds(-speedAll,-halfSpeed,-speedAll,-halfSpeed);
    movinforward = false;
    movinbackward = true;
  }
}

void avoidFront(){
  inputControl('x');
  delay(500);
  inputControl('s');
  delay(2000);
  inputControl('x');
  movinforward = false;
}

void avoidBack(){
  inputControl('x');
  delay(500);
  inputControl('w');
  delay(2000);
  inputControl('x');
  movinbackward = false;
}


void loop() {

  //back bumper
  frontbumpint = digitalRead(frontbumper);
  backbumpint = digitalRead(backbumper);

  //USB serial
  if (Serial.available() > 0) {
    // read the incoming byte:
    serialIn = Serial.read();
    
    // say what you got:
    Serial.print("I received: ");
    Serial.println(serialIn);

    inputControl(serialIn);
  }

  //Bluetooth Serial
  if(mySerial.available() ){
    // read the incoming byte:
    serialIn = mySerial.read();
    
    // say what you got:
    mySerial.print("I received: ");
    mySerial.println(serialIn);

    inputControl(serialIn);
  }

  //command for bumpers
  if( frontbumpint == 0 && movinforward ){
    avoidFront();
  }
  if( backbumpint == 0 && movinbackward ){
    avoidBack();
  }
}
