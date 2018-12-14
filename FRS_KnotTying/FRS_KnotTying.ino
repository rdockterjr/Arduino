// define constants

// I/O
#define ENABLEPIN 9       // output pin
#define MOTFORWARD 3      // output pin
#define MOTBACK 4         // output pin
#define POTVALUE 1        // analog pin for Pot from servo
#define loadCell 0        // analog pin for loadcell
#define startButton 2     //assign pin 2 to startButton
#define RING_TOUCH 5   // input


//State Machine
#define IDLE_STATE 0              
#define KNOT_TYING_IN_PROGRESS 1
#define PULLING_TEST 2
#define RETURN 3


#define PULLING_TEST_CURRENT   255  // Determined experimentally to give 900g pull force.  This could be replaced by a pulling force control loop.
#define UNACCEPTABLE_POT_VALUE 256 // Distance beyond which is fail (in pot units)
#define START_POINT            327   // Zero pose (in pot units)
#define PULLING_TEST_LENGTH    2000// Time allowed for pulling before assuming the knot held, in milliseconds
#define POTENTIOMETER_FIT_A0 11.371
#define POTENTIOMETER_FIT_A1 0.2489
#define LOAD_CELL_FIT_A0     -1449.5
#define LOAD_CELL_FIT_A1     4.0577
#define LOAD_CELL_X 13.26            // UNITS: mm
#define LOAD_CELL_Y 39.73            // UNITS: mm
#define HALF_RING_RADIUS 5.95        // UNITS: mm

#define RETURN_TIME_LIMIT 500 // time for servo arm to return to start pot position (ms)

//Uncomment the following line for verbose debug output to serial
//#define DEBUG_OUTPUT

long knot_tying_start_time;
long knot_tying_time_elapsed=0;
long pulling_test_start_time;
long return_start_time;
float start_position=0;
float end_position=0;
float peak_force=0;
int state=IDLE_STATE; // inital state
boolean ring_state_end;
boolean ring_state_start;
float end_dist = 0;
float start_dist = 0;
float end_position_radians;
float start_position_radians;
float servo_arm_x;
float servo_arm_y;
float start_distance;
float end_distance;

//PID loop variables
#define INTEGRAL_WINDUP_LIMIT 2000
#define MAX_PWM 255
int integral_error,last_error;
int kp,kd,ki;
long last_time;

void setup()
{
  pinMode(ENABLEPIN,OUTPUT);
  pinMode(MOTFORWARD, OUTPUT);
  pinMode(MOTBACK, OUTPUT);
  pinMode(POTVALUE, INPUT);
  pinMode(startButton, INPUT);    // start/stop switch
  pinMode(loadCell, INPUT);       // loadCell
  pinMode(RING_TOUCH, INPUT);
  last_time=millis();
  integral_error=0;
  last_error=0;
  kp=4;
  kd=0;
  ki=200;
  Serial.begin(9600);
}

float degrees_to_radian(int degrees_)
{
  float radian = (2*PI*degrees_)/360;
  return radian;
}

float calculate_distance(float rad)
{
 // covert from polar to cart coord
 float R = LOAD_CELL_Y;
 servo_arm_x = (float)(R*cos(rad));  // x position of servo arm with base of servo arm as 0, 0 origin
 servo_arm_y = (float)(R*sin(rad));  // y position of servo arm with base of servo arm as 0, 0 origin
 // distance formula and subtract 2 radius from distance
 float distance = sqrt((LOAD_CELL_X-servo_arm_x)*(LOAD_CELL_X-servo_arm_x)+(LOAD_CELL_Y-servo_arm_y)*(LOAD_CELL_Y-servo_arm_y))- (2.0*HALF_RING_RADIUS);
 return distance;
}

int degrees_to_adc(float degree) // convert from degrees to raw pot value
{
  // From an excel file blah.xlsx
  int adc= (degree-(POTENTIOMETER_FIT_A0))/(POTENTIOMETER_FIT_A1); 
  //Make sure we are in a valid range 
  if(adc<0){
    adc=0;
  }
  else if(adc>1023){
    adc=1023;
  }
  return adc;// returns raw analog
}

float adc_to_degrees(int adc)  // convert from raw pot value to degrees
{
  return (POTENTIOMETER_FIT_A1)*adc+(POTENTIOMETER_FIT_A0);  // returns degrees
}

// pwm code to turn the motor forward/backwards until its pot value is at target value
void motor_go(int commanded_speed)
{
  ///Make sure we have a valid command min-max pwm values
  //  Serial.print("commanded_speed raw:");
  //  Serial.println(commanded_speed);
  if(commanded_speed<-MAX_PWM)
    commanded_speed=-MAX_PWM; // pwm max and min
  if(commanded_speed>MAX_PWM)
    commanded_speed=MAX_PWM;

  if(commanded_speed<0)
  {
    digitalWrite(MOTBACK,HIGH);
    digitalWrite(MOTFORWARD,LOW);
    analogWrite(ENABLEPIN,-commanded_speed);
  }
  else{
    digitalWrite(MOTBACK,LOW);
    digitalWrite(MOTFORWARD,HIGH);
    analogWrite(ENABLEPIN,commanded_speed);
  }
  //  Serial.print("commanded_speed final:");
  //  Serial.println(commanded_speed);
} // end of void motor_go(int commanded_speed)

void pid_loop(int target)
{
  int pot_value=analogRead(POTVALUE);          // assign pot value
  int error=pot_value-target;                  // determine difference from current pot position to target position (V) for pot value
  integral_error+=error;                       // add error to integral_error
  if(integral_error<-INTEGRAL_WINDUP_LIMIT)    // set min and max limits for integral error
    integral_error=-INTEGRAL_WINDUP_LIMIT;     // if less than 20 than 20 
  if(integral_error>INTEGRAL_WINDUP_LIMIT)     
    integral_error=INTEGRAL_WINDUP_LIMIT;      // if greater than 20 than 20
  int dt=millis()-last_time;                   // calculate time difference
  last_time=millis();
  // calculate pwm value for pot
  int commanded_speed=kp*error+kd*(error-last_error)/dt+((ki*integral_error)*dt)/1000;
  motor_go(commanded_speed);                   // turn motor
  //  Serial.print("Error=");
  //  Serial.print(error);
  //  Serial.print("commanded_speed=");
  //  Serial.println(commanded_speed);
} // end of void pid_loop(int target)



// =============================================================
// REPORT DATA 
// =============================================================
void test_is_finished(boolean result)
{
  return_start_time = millis();
  end_position=adc_to_degrees(analogRead(POTVALUE));       // METRIC #3. End position
  end_position_radians = degrees_to_radian(end_position);
  end_distance = calculate_distance(end_position_radians);
  // calculate distance
  
  if (digitalRead(RING_TOUCH) == LOW)                        // METRIC #5. End touching?
  {
    ring_state_end = true;                                // rings are touching
  } 
  else
  {
    ring_state_end = false;                                  /// rings are not touching
  }
#ifdef DEBUG_OUTPUT
  Serial.print("test_is_finished --> RETURN");
#endif
  state=RETURN;
  //Serial.println("Data");
  Serial.print(knot_tying_time_elapsed); 
  Serial.print(",");
  Serial.print(start_position);    // units: mm 
  Serial.print(",");
  Serial.print(end_position);      // units: mm
  Serial.print(",");
  if (ring_state_start)
  {
    Serial.print("true"); // touch_initial  
  }
  else
  {
    Serial.print("false"); // touch_initial
  }
  Serial.print(",");
  if (ring_state_end)
  {
    Serial.print("true"); // touch_final  
  }
  else
  {
    Serial.print("false"); // touch_final
  }
  Serial.print(",");
  Serial.print(peak_force);
  Serial.println();

} // end of void test_is_finished(boolean passed)

void loop()
{
  int target=(450);                                // input raw pot value between (0-1023) maps to 0-179
  //pid_loop(degrees_to_adc(target_deg));

  //======== 1. state==IDLE_STATE =================
  if(state==IDLE_STATE)
  {
#ifdef DEBUG_OUTPUT
    Serial.println("current state = idle_state");
#endif
    int current_pot_value = analogRead(POTVALUE);
#ifdef DEBUG_OUTPUT
    Serial.println(current_pot_value);
#endif
    motor_go(0);
    //pid_loop(START_POINT);                         // hold servo_arm at start pot position
    //check to see if they pressed the button
    if(digitalRead(startButton) == LOW)
    {
      while (digitalRead(startButton) == LOW)
      {
        delay(10);
      }
      state = KNOT_TYING_IN_PROGRESS;
      knot_tying_start_time=millis();
#ifdef DEBUG_OUTPUT
      Serial.println("IDLE --> KNOT_TYING_IN_PROGRESS");
#endif
    }

  }
  //======== 2. state==KNOT_TYING_IN_PROGRESS =================
  else if(state==KNOT_TYING_IN_PROGRESS)
  {
#ifdef DEBUG_OUTPUT
    Serial.println(" state = KNOT_TYING_IN_PROGRESS ");
#endif
    pid_loop(START_POINT);  ///Is this really what we want?
    //check to see if they pressed the button
    if(digitalRead(startButton) == LOW)
    {
      while (digitalRead(startButton) == LOW)
      {
        delay(10);
      }

#ifdef DEBUG_OUTPUT
      Serial.println("KNOT_TYING_IN_PROGRESS --> PULLING_TEST"); 
#endif
      knot_tying_time_elapsed=millis()-knot_tying_start_time;   // METRIC #1 TOTAL TIME
      start_position=adc_to_degrees(analogRead(POTVALUE));      // METRIC #2 START POSITION, units: degrees
      start_position_radians = degrees_to_radian(start_position);
      start_distance = calculate_distance(start_position_radians);
      if (digitalRead(RING_TOUCH) == LOW)                    // METRIC #4. START TOUCHING?
      {
        ring_state_start = true;                                  //rings are touching
      }
      else
      {
        ring_state_start = false;                                // rings are not touching
      }
      pulling_test_start_time=millis();
      peak_force=0;
      state=PULLING_TEST;
    }
  }

  //======== 3. (state==PULLING_TEST) =================
  else if(state==PULLING_TEST)
  {
#ifdef DEBUG_OUTPUT
    Serial.println(" state = PULLING_TEST");
#endif
    motor_go(PULLING_TEST_CURRENT);        // PULLING_TEST_CURRENT
    // assign raw data read to sensorValue
    int sensorValue = analogRead(loadCell);

    float force;
    // force equation based on weight calibration
    force = (LOAD_CELL_FIT_A1)*(sensorValue) + (LOAD_CELL_FIT_A0); // read load cell value and convert to gram force see Load_cell_calibration.xlsx file
    if(force>peak_force)
    {
      peak_force=force;                                           // METRIC #6. Peak Force
    }
    // if pot value difference is greater than a certain value than 
    // eye screw are too far apart and not touching
    if(analogRead(POTVALUE)<UNACCEPTABLE_POT_VALUE)
    { 
#ifdef DEBUG_OUTPUT
      Serial.println("PULLING_TEST --> RETURN1");
#endif
      // report data, set state = return
      test_is_finished(false);
      //test is failed!
    }
    // if time pulled does not exceed the raw pot value difference
    // than knot tying is strong and stationary during pull
    if(millis()-pulling_test_start_time>PULLING_TEST_LENGTH)
    {
#ifdef DEBUG_OUTPUT
      Serial.println("PULLING_TEST --> RETURN2");
#endif
      // report data, set state = return
      test_is_finished(true);
      //test is pass!
    }

  }
  //======== 4. state==RETURN =================
  else if(state==RETURN){
    pid_loop(START_POINT);  // return arm to start position
#ifdef DEBUG_OUTPUT
    Serial.println("state = RETURN");
#endif
    if(analogRead(POTVALUE)==target || millis()-return_start_time>RETURN_TIME_LIMIT)
    {
#ifdef DEBUG_OUTPUT
      Serial.println("RETURN --> IDLE");
#endif
      state=IDLE_STATE;
    }
  }
} // end of void loop
