#include <MPU9250.h>


//basic code to read imu roll pitch and yaw using MPU9250 library

#include <Wire.h>

#include <SPI.h>

#include <MPU9250.h>
#include <quaternionFilters.h>


////////////////////////////////////////////////////////////VARIABLES //////////////////////////////////////////

// Pin definitions
int intPin = 12;  // These can be changed, 2 and 3 are the Arduinos ext int pins
int myLed  = 13;  // Set up pin 13 led for toggling

MPU9250 myIMU;

float roll_current = 0.0;
float yaw_current = 0.0;
float pitch_current = 0.0;
bool imugood = false;
bool newimudata = false;



///////////////////////////////////////////////////////////////HELPER Functions///////////////////////////////

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




////////////////////////////////////////////////////////////////SETUP AND LOOP////////////////////////////////////

void setup()
{
  Wire.begin();
  // TWBR = 12;  // 400 kbit/sec I2C speed
  Serial.begin(38400);

  // Set up the interrupt pin, its set as active high, push-pull
  pinMode(intPin, INPUT);
  digitalWrite(intPin, LOW);
  pinMode(myLed, OUTPUT);

  for(int ii = 0; ii <3; ii++){
    digitalWrite(myLed, HIGH);
    delay(200);
    digitalWrite(myLed,LOW);
    delay(200);
  }
  
  imugood = initializeIMU9250();
}


void loop()
{
  //see if we have new data
  newimudata = readIMUdata();

  //now we update the quaternions
  updateQuaternions();
  
  // Serial print and/or display at 0.5 s rate independent of data rates
  myIMU.delt_t = millis() - myIMU.count;

  // update LCD once per half-second independent of read rate
  if (myIMU.delt_t > 500)
  {

    getCurrentValues(roll_current, pitch_current, yaw_current);

    Serial.print("roll: ");
    Serial.print(roll_current,2);
    Serial.print(" pitch: ");
    Serial.print(pitch_current,2);
    Serial.print(" yaw: ");
    Serial.println(yaw_current,2);
    
    myIMU.count = millis();
    myIMU.sumCount = 0;
    myIMU.sum = 0;
  } 
}



