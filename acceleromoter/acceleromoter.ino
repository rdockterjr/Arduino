int xpin = A1;   
int ypin = A2; 
int zpin = A3; 
int LEDpin = 2;

int x_acc = 0;
int y_acc = 0;
int z_acc = 0;

int avg_x = 513;
int avg_y = 521;
int avg_z = 629;

int diffx = 0;
int diffy = 0;
int diffz = 0;
int cnt = 0;

float analog2mps = 0.08376; 

void setup()
{
  Serial.begin(9600);          //  setup serial
  
  pinMode(LEDpin,OUTPUT);
}

void loop()
{
  
  x_acc = analogRead(xpin); 
  y_acc = analogRead(ypin); 
  z_acc = analogRead(zpin); 

  diffx = x_acc - avg_x;
  diffy = y_acc - avg_y;
  diffz = z_acc - avg_z;
  
  Serial.print(x_acc); 
  Serial.print(","); 
  Serial.print(y_acc); 
  Serial.print(","); 
  Serial.println(z_acc); 
  delay(30);
}
