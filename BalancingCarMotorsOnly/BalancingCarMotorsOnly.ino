//Needs this encoder library
//http://www.pjrc.com/teensy/td_libs_Encoder.html
#include <Encoder.h>

//variables for directions
#define MOTORFORWARD 1
#define MOTORBACKWARD -1
#define MOTORSTOP 0
#define PWM_MAX 254
#define CHANGE_LIMIT 50

//PINS FOR L298N
// LEFT MOTOR A
int enA = 6;
int in1 = 7;
int in2 = 8;
// RIGHT MOTOR B
int enB = 11;
int in3 = 9;
int in4 = 10;

//Odomoetry Variable
float X_Pose_Estimate, Y_Pose_Estimate, Theta_Pose_Estimate;
//Robot constraints (actually measured)
float WHEEL_BASE       = 0.170; //meters
float WHEEL_RADIUS     = 0.0325; //meters
float WHEEL_FUDGE      = 1.00; //ratio
unsigned long last_time_odom;
float dirFlipL = 1.0; //because left wheel is backwards
float dirFlipR = 1.0; // right wheel goes correct direction



//Velocity Variables
long oldLeft  = 0;
long oldRight = 0;
long newLeft, newRight, diffLeft, diffRight;
float dt = 1.0; // 1hz
unsigned long last_timeL, last_timeR;
float dLeft = 0.0;
float dRight = 0.0;
float MotorCommandL,MotorCommandR;
float Error_Prev_Left, Error_Prev_Right;
float Error_Now_Left, Error_Now_Right;
float Setpoint_ActualLeft,Setpoint_ActualRight;

//Specify the links and initial tuning parameters
float Kp = 1.0;
float Ki = 0.00003;
float Kd = 0.0;
int EncPerRev = 1120; //1920; //32 cpr x 35:1 gear ratio
float EncPerRad;
float RadPerEncUs;

//Encoders 
Encoder EncMotorL(2,4);
Encoder EncMotorR(3,5);


//status led
int ledpin = 13;


///////////////////////////////////MAIN SETUP ////////////////////////////
void setup()
{
  // Some maths
  EncPerRad=(float(EncPerRev)/(2.0*3.14159));
  RadPerEncUs=(1e6/EncPerRad);

  //serial for debug
  Serial.begin(9600);
  
  // set all the motor control pins to outputs
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  pinMode(ledpin,OUTPUT);

  //get starting encoder
  oldLeft = EncMotorL.read();
  oldRight = EncMotorR.read();
  last_timeL = micros() - 1;
  last_timeR = micros() - 1;

  //init odom
  last_time_odom = micros() - 1;
  X_Pose_Estimate = 0.0;
  Y_Pose_Estimate = 0.0;
  Theta_Pose_Estimate = 0.0;

  //initialize
  dLeft = 0.0;
  dRight = 0.0;
  Setpoint_ActualLeft = 0.0;
  Setpoint_ActualRight = 0.0;
  MotorCommandL = 0.0;
  MotorCommandR = 0.0;
  
  //speed init
  setMotorPWM(0, 0);
}


///////////////////////HELPER FUNCTIONS /////////////////////////////////////////

//keep angle between +pi/-pi
void boundTheta(float &angle)
{
  //modulo doesnt work on floats in arduino WTF
  while (angle <= -PI){ angle += 2*PI; }
  while (angle > PI){ angle -= 2*PI; }
}

//set target velocities
void set_target(float target_left,float target_right)
{
  Setpoint_ActualLeft = dirFlipL*target_left; 
  Setpoint_ActualRight = dirFlipR*target_right;
}


//set direction in L298 chip
void setDirection(int directionL, int directionR)
{
  //Left Motor
  if(directionL == MOTORFORWARD){
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
  }
  else if(directionL == MOTORBACKWARD){
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
  }
  else if(directionL == MOTORSTOP){
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
  }
  
  //Right Motor
  if(directionR == MOTORFORWARD){
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);
  }
  else if(directionR == MOTORBACKWARD){
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);
  }
  else if(directionR == MOTORSTOP){
    digitalWrite(in3, LOW);
    digitalWrite(in4, LOW);
  }
  
}

//change the enable pins to change direction
void setMotorPWM(int pwmL, int pwmR)
{
  int dirL = MOTORFORWARD;
  int dirR = MOTORFORWARD;
  int magL = abs(pwmL);
  int magR = abs(pwmR);

  //deal with directions
  if(pwmL < 0){
    dirL = MOTORBACKWARD;
  }
  else if(pwmL == 0){
    dirL = MOTORSTOP;
  }
  if(pwmR < 0){
    dirR = MOTORBACKWARD;
  }
  else if(pwmR == 0){
    dirR = MOTORSTOP;
  }
  setDirection(dirL, dirR);

  //Left Motor
  analogWrite(enA, magL);
  
  //Right Motor
  analogWrite(enB, magR);
}


//PI control to set motor PWM to correct value to keep desired velocity
void VelocityController()
{
  //Get new values of the wheel encoders
  //get current encoder count and delta time
  newLeft = EncMotorL.read();
  dt=micros()-last_timeL;
  last_timeL = micros();
  //get angular velocity
  diffLeft = newLeft - oldLeft;
  dLeft = (float(diffLeft) / dt) * RadPerEncUs;

  //get current encoder count and delta time
  newRight = EncMotorR.read();
  dt=micros()-last_timeR;
  last_timeR = micros();
  //get angular velocity
  diffRight = newRight - oldRight;
  dRight = (float(diffRight) / dt) * RadPerEncUs;

  //get error from command velocity to actual
  Error_Now_Left = Setpoint_ActualLeft - dLeft ;
  Error_Now_Right = Setpoint_ActualRight - dRight ;

  //velocity PID
  float changeL = Error_Now_Left*Ki*dt + Kp*(Error_Now_Left-Error_Prev_Left);
  float changeR = Error_Now_Right*Ki*dt + Kp*(Error_Now_Right-Error_Prev_Right);
  if(changeL>CHANGE_LIMIT){changeL=CHANGE_LIMIT;}
  if(changeL<-CHANGE_LIMIT){changeL=-CHANGE_LIMIT;}
  if(changeR>CHANGE_LIMIT){changeR=CHANGE_LIMIT;}
  if(changeR<-CHANGE_LIMIT){changeR=-CHANGE_LIMIT;}

  //Motor commandings in PWM units -255 to +255
  MotorCommandL+=changeL;
  MotorCommandR+=changeR;
  
  //Avoid integral windup, if you are failing to meet target 
  //even with max power.
  if (MotorCommandL > PWM_MAX) {MotorCommandL= PWM_MAX;}
  if (MotorCommandL <-PWM_MAX) {MotorCommandL=-PWM_MAX;}
  if (MotorCommandR > PWM_MAX) {MotorCommandR= PWM_MAX;}
  if (MotorCommandR <-PWM_MAX) {MotorCommandR=-PWM_MAX;}
  
  //Move the new value into the past
  oldLeft=newLeft;
  oldRight=newRight;
  Error_Prev_Left=Error_Now_Left;
  Error_Prev_Right=Error_Now_Right;

  //Send the command to the motor!
  setMotorPWM(int(MotorCommandL), int(MotorCommandR));

}


//differential drive equaitons
void Compute_Odometry()
{
  //since we last odomed
  dt = (micros()-last_time_odom) / 1e6; //convert this to seconds cuz m/s
  last_time_odom = micros();
  
  /// Multiply by radius to go from radians/s to m/s
  /// Correct for the fact that the two wheels aren't the same size.
  //dLeft and dRight are globals
  float u_l = dLeft*WHEEL_RADIUS*WHEEL_FUDGE;
  float u_r = dRight*WHEEL_RADIUS;

  /// Calculate linear and angular velocity in robot frame
  float vx = 0.5*(u_l + u_r);
  float vy = 0;
  float vth = (u_r - u_l)/WHEEL_BASE;
  
  /// Propagate the robot using basic odom
  X_Pose_Estimate += vx*cos(Theta_Pose_Estimate) * dt;
  Y_Pose_Estimate += vx*sin(Theta_Pose_Estimate) * dt;
  Theta_Pose_Estimate += vth * dt;
  //keep theta between +/-PI
  boundTheta(Theta_Pose_Estimate);
}

/////////////////////////////////////////////MAIN LOOP /////////////////////////////////////////////////////////////

void loop()
{
  //check for new commands
  if (Serial.available() > 0) {
          // read the incoming byte:
          float gotit = 3.0;
          int inbyte = Serial.read();

          if(inbyte == 119){
            //forward (w)
            Serial.println("I received W");
            set_target(gotit,gotit);
          }
          if(inbyte == 97){
            //left (a)
            Serial.println("I received A");
            set_target(-gotit,gotit);
          }
          if(inbyte == 115){
            //backward (s)
            Serial.println("I received S");
            set_target(0.0,0.0);
          }
          if(inbyte == 100){
            // right (d)
            Serial.println("I received D");
            set_target(gotit,-gotit);
          }
  }
  
  //make sure our velocity is legit every time
  VelocityController();

  Serial.print("vel L: ");
  Serial.print(dLeft);
  Serial.print(" vel R ");
  Serial.println(dRight);

  //compute our odometry
  Compute_Odometry();
//  Serial.print("POSE: ");
//  Serial.print(X_Pose_Estimate);
//  Serial.print(", ");
//  Serial.print(Y_Pose_Estimate);
//  Serial.print(", ");
//  Serial.println(Theta_Pose_Estimate);

  delay(100);
}
