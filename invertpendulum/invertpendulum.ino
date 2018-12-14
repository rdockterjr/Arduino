//declare pins
int in1 = 12;
int in2 = 13;
int enab = 11;
int potpin = 0;

//declare control vars
int center = 530;
int potval = 0;
int error = 0;
int k_p = 400;
int k_i = 200;
int pwm_val = 0;
int i_term = 0;
int loop_cnt = 0;
int ts = 0.03;


void setup(){
  pinMode(in1,OUTPUT);
  pinMode(in2,OUTPUT);
  pinMode(enab,OUTPUT); //pwm 11
 Serial.begin(9600); 
}


void loop(){
  potval = analogRead(potpin);
  error = potval - center;
  i_term = ts*(i_term+error);
  
  pwm_val = error * k_p + i_term * k_i;
  
  if(error < 0){
    //flip
    digitalWrite(in1,HIGH);
    digitalWrite(in2,LOW);
  }
  else{
    digitalWrite(in1,LOW);
    digitalWrite(in2,HIGH);
  }
  
  if(pwm_val > 255){
   pwm_val = 255; 
  }
  if(pwm_val < 0){
     pwm_val = 0; 
  }
  
  analogWrite(enab,pwm_val);
  
  delay(30);
  
  loop_cnt++;
  
  Serial.println(potval);
  
//  delay(5000);
//  
//  digitalWrite(enab,HIGH);
//  digitalWrite(in1,LOW);
//  digitalWrite(in2,HIGH);
//  
//  delay(5000);
//  digitalWrite(enab,LOW);
//  digitalWrite(in1,LOW);
//  digitalWrite(in2,LOW);
//  
//  delay(5000);
}

