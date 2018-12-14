//Needs this encoder library
//http://www.pjrc.com/teensy/td_libs_Encoder.html
#include <Encoder.h>
#include <Wire.h>
#include <SPI.h>
#include "quaternionFilters.h"
#include "MPU9250.h"

//variables for directions
#define MOTORFORWARD 1
#define MOTORBACKWARD -1
#define MOTORSTOP 0
#define PWM_MAX 254
#define CHANGE_LIMIT 50

//variables for IMU
// Pin definitions
int intPin = 12;  // These can be changed, 2 and 3 are the Arduinos ext int pins
//status led
int ledpin = 13;
MPU9250 myIMU;

float roll_current = 0.0;
float yaw_current = 0.0;
float pitch_current = 0.0;
bool imugood = false;
bool newimudata = false;

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

//balance variables
float commandVelocity = 0.0;
float errorBalance = 0.0;
float k_pitch2vel = 2;

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

//Specify the ratios and initial tuning parameters
float Kp = 1.0;
float Ki = 0.00005;
float Kd = 0.0;
int EncPerRev = 1120; //1920; //32 cpr x 35:1 gear ratio
float EncPerRad;
float RadPerEncUs;

//Encoders 
Encoder EncMotorL(2,4);
Encoder EncMotorR(3,5);


/////////////////////////////////////////////// IMU HELPER Functions///////////////////////////////


//initilize the IMU class
bool initializeIMU9250(){
  
  // Read the WHO_AM_I register, this is a good test of communication
  byte c = myIMU.readByte(MPU9250_ADDRESS, WHO_AM_I_MPU9250);
  Serial.print("MPU Hex: "); Serial.println(c, HEX);
  Serial.print(" I should be "); Serial.println(0x73, HEX);
  
  if (c == 0x73) // WHO_AM_I should always be 0x73
  {
    Serial.println("MPU9250 online...");

    // Start by performing self test and reporting values
    myIMU.MPU9250SelfTest(myIMU.SelfTest);
    float xacctest = myIMU.SelfTest[0];
    float yacctest = myIMU.SelfTest[1];
    float zacctest = myIMU.SelfTest[2];
    float xgyrotest = myIMU.SelfTest[3];
    float ygyrotest = myIMU.SelfTest[4];
    float zgyrotest = myIMU.SelfTest[5];

    // Calibrate gyro and accelerometers, load biases in bias registers
    myIMU.calibrateMPU9250(myIMU.gyroBias, myIMU.accelBias);

    // Initialize device for active mode read of acclerometer, gyroscope, and
    // temperature
    myIMU.initMPU9250();
    
    // Read the WHO_AM_I register of the magnetometer, this is a good test of
    // communication
    byte d = myIMU.readByte(AK8963_ADDRESS, WHO_AM_I_AK8963);
    Serial.print("Magnetometer hex: "); Serial.println(d, HEX);
    Serial.print(" I should be "); Serial.println(0x48, HEX);

    // Get magnetometer calibration from AK8963 ROM
    // Initialize device for active mode read of magnetometer
    myIMU.initAK8963(myIMU.magCalibration);
    
    //calibration values
    float xmagtest = myIMU.magCalibration[0];
    float ymagtest = myIMU.magCalibration[1];
    float zmagtest = myIMU.magCalibration[2];

    return true;
  } // if (c == 0x71)
  else
  {
    return false;
  }
}


// If intPin goes high, all data registers have new data
// On interrupt, check if data ready interrupt
bool readIMUdata(){
  
  if (myIMU.readByte(MPU9250_ADDRESS, INT_STATUS) & 0x01)
  {  
    myIMU.readAccelData(myIMU.accelCount);  // Read the x/y/z adc values
    myIMU.getAres();
    
    // Now we'll calculate the accleration value into actual g's
    // This depends on scale being set
    myIMU.ax = (float)myIMU.accelCount[0]*myIMU.aRes; // - accelBias[0];
    myIMU.ay = (float)myIMU.accelCount[1]*myIMU.aRes; // - accelBias[1];
    myIMU.az = (float)myIMU.accelCount[2]*myIMU.aRes; // - accelBias[2];
    
    myIMU.readGyroData(myIMU.gyroCount);  // Read the x/y/z adc values
    myIMU.getGres();
    
    // Calculate the gyro value into actual degrees per second
    // This depends on scale being set
    myIMU.gx = (float)myIMU.gyroCount[0]*myIMU.gRes;
    myIMU.gy = (float)myIMU.gyroCount[1]*myIMU.gRes;
    myIMU.gz = (float)myIMU.gyroCount[2]*myIMU.gRes;
    
    myIMU.readMagData(myIMU.magCount);  // Read the x/y/z adc values
    myIMU.getMres();
    // User environmental x-axis correction in milliGauss, should be
    // automatically calculated
    myIMU.magbias[0] = +470.;
    // User environmental x-axis correction in milliGauss TODO axis??
    myIMU.magbias[1] = +120.;
    // User environmental x-axis correction in milliGauss
    myIMU.magbias[2] = +125.;
    
    // Calculate the magnetometer values in milliGauss
    // Include factory calibration per data sheet and user environmental
    // corrections
    // Get actual magnetometer value, this depends on scale being set
    myIMU.mx = (float)myIMU.magCount[0]*myIMU.mRes*myIMU.magCalibration[0] -
               myIMU.magbias[0];
    myIMU.my = (float)myIMU.magCount[1]*myIMU.mRes*myIMU.magCalibration[1] -
               myIMU.magbias[1];
    myIMU.mz = (float)myIMU.magCount[2]*myIMU.mRes*myIMU.magCalibration[2] -
               myIMU.magbias[2];
    
    return true;
  }
  else{
    return false;
  }
}


//call to IMU class to update quaternions
void updateQuaternions(){
  // Must be called before updating quaternions!
  myIMU.updateTime();

  //  MadgwickQuaternionUpdate(ax, ay, az, gx*PI/180.0f, gy*PI/180.0f, gz*PI/180.0f,  my,  mx, mz);
  MahonyQuaternionUpdate(myIMU.ax, myIMU.ay, myIMU.az, myIMU.gx*DEG_TO_RAD,
                         myIMU.gy*DEG_TO_RAD, myIMU.gz*DEG_TO_RAD, myIMU.my,
                         myIMU.mx, myIMU.mz, myIMU.deltat);
}


//store current acceleration, and gyro to vars
void getCurrentValues(float &rollin, float &pitchin, float &yawin){
  
  // acceleration values in milli-G's
  int ax_now = (int)1000*myIMU.ax;
  int ay_now = (int)1000*myIMU.ay;
  int az_now = (int)1000*myIMU.az;

  // gyro values in degree/sec
  float gx_now = myIMU.gx;
  float gy_now = myIMU.gy;
  float gz_now = myIMU.gz;

  // mag values in mG
  int mx_now = (int)myIMU.mx;
  int my_now = (int)myIMU.my;
  int mz_now = (int)myIMU.mz;

  //quaternion raw values
  float q0_now = *getQ();
  float qx_now = *(getQ() + 1);
  float qy_now = *(getQ() + 2);
  float qz_now = *(getQ() + 3);

  //quaterniosn to euler angles
  myIMU.yaw   = atan2(2.0f * (*(getQ()+1) * *(getQ()+2) + *getQ() *
                *(getQ()+3)), *getQ() * *getQ() + *(getQ()+1) * *(getQ()+1)
                - *(getQ()+2) * *(getQ()+2) - *(getQ()+3) * *(getQ()+3));
  myIMU.pitch = -asin(2.0f * (*(getQ()+1) * *(getQ()+3) - *getQ() *
                *(getQ()+2)));
  myIMU.roll  = atan2(2.0f * (*getQ() * *(getQ()+1) + *(getQ()+2) *
                *(getQ()+3)), *getQ() * *getQ() - *(getQ()+1) * *(getQ()+1)
                - *(getQ()+2) * *(getQ()+2) + *(getQ()+3) * *(getQ()+3));
  myIMU.pitch *= RAD_TO_DEG;
  myIMU.yaw   *= RAD_TO_DEG;
  
  // Declination of Minneapolis (44° 58' 49" N 93° 13' 48" W) is
  //   0°16' E  ± 0° 24' (or 0.3°) on 11-23-2016
  // - http://www.ngdc.noaa.gov/geomag-web/#declination
  myIMU.yaw   -= 0.3;
  myIMU.roll  *= RAD_TO_DEG;
  
  yawin = myIMU.yaw;
  pitchin = myIMU.pitch;
  rollin = myIMU.roll;

  //data rate in hz
  float rate_now = (float)myIMU.sumCount/myIMU.sum;

  //temperature
  myIMU.tempCount = myIMU.readTempData();  // Read the adc values
  // Temperature in degrees Centigrade
  myIMU.temperature = ((float) myIMU.tempCount) / 333.87 + 21.0;
  float temp_now = myIMU.temperature;
  
}


//////////////////////////////////////////MOTOR HELPER FUNCTIONS /////////////////////////////////////////

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


/////////////////////////////////////////////////  MAIN SETUP //////////////////////////////////////
void setup()
{
  // Some maths
  EncPerRad=(float(EncPerRev)/(2.0*3.14159));
  RadPerEncUs=(1e6/EncPerRad);

  //serial for debug
  Serial.begin(9600);

  //pins for imu
  Wire.begin();
  pinMode(intPin, INPUT); // Set up the interrupt pin, its set as active high, push-pull
  digitalWrite(intPin, LOW);
  
  // set all the motor control pins to outputs
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  pinMode(ledpin,OUTPUT);

  for(int ii=0; ii <3; ii++){
    digitalWrite(ledpin,HIGH);
    delay(100);
    digitalWrite(ledpin,LOW);
    delay(100);
  }

  //initialize the IMU
  imugood = initializeIMU9250();

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



/////////////////////////////////////////////MAIN LOOP /////////////////////////////////////////////////////////////

void loop()
{
  //see if we have new data
  newimudata = readIMUdata();

  //now we update the quaternions
  updateQuaternions();
  
  // Serial print and/or display at 0.5 s rate independent of data rates
  myIMU.delt_t = millis() - myIMU.count;

  //get the current pitch
  getCurrentValues(roll_current, pitch_current, yaw_current);

  // ACTUAL CONTROLLER
  errorBalance = 0 - pitch_current; //for now assume we want to keep at zero
  //translate this is into velocity
  commandVelocity = errorBalance*k_pitch2vel;

  //command wheel velocity
  set_target(commandVelocity,commandVelocity);
  
  //make sure our velocity is legit every time
  VelocityController();

  //compute our odometry
  Compute_Odometry();

  // update LCD once per half-second independent of read rate
  if (myIMU.delt_t > 200)
  {
    Serial.print(" pitch: ");
    Serial.print(pitch_current,2);
    Serial.print("vel L: ");
    Serial.print(dLeft);
    Serial.print(" vel R ");
    Serial.print(dRight);
  //  Serial.print("POSE: ");
  //  Serial.print(X_Pose_Estimate);
  //  Serial.print(", ");
  //  Serial.print(Y_Pose_Estimate);
  //  Serial.print(", ");
  //  Serial.print(Theta_Pose_Estimate);
    Serial.println(" ");
    
    myIMU.count = millis();
    myIMU.sumCount = 0;
    myIMU.sum = 0;
  } 

  delay(5);
}


