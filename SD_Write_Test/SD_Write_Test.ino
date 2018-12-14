#include <SD.h>
#include <string.h>
#include <stdlib.h>

File SD_Print_File;
String Folder_Name = "/FRS_DATA/";
String ParticipantID = "";
//Initials plus 3 digit code eg: RD123

boolean SD_Initialized = false;
boolean Folder_Created = false;
boolean File_Created = false;
boolean ID_Received = false;
unsigned long Time_Stamp = 0;
int Task1Int = 1;
int Task2Int = 2;

int retval = 70069;

void setup(){
  ParticipantID = "RD356";
 //must stay
 pinMode(53,OUTPUT);
  Serial.begin(9600);
  while(!Serial){
  }
  
  Serial.println("Initiliazing");
  
  if(!SD.begin(8)){
   Serial.println("SD failed to Initialize");
   return; 
  }
  else{
    SD_Initialized = true;
    ID_Received = true;
  }
  Serial.println("SD Initlialized");
  
  create_folder();
  
  create_file(Task2Int);
  int i = 0;
  int readval = 0;
  if(File_Created){
   Serial.println("printing to SD");
    for(i = 0; i <= 1000; i += 1){
      readval = analogRead(0);
      Time_Stamp = millis();
      PrintToFile(3, Time_Stamp, i, readval, 72, 89);
    }
    
  }
  SD_Print_File.close();
  Serial.println("Exiting");
}


void loop(){
}


void create_folder(){
  char Folder_Char[50];
  Folder_Name.concat(ParticipantID);
  Folder_Name.toCharArray(Folder_Char, 50); 
  Serial.println(Folder_Char);
  if(SD_Initialized && ID_Received){
   if(SD.mkdir(Folder_Char)){
     Folder_Created = true;
     Serial.println("Folder Created");
   }
  }
}

void create_file(int tasknum){
  char File_Char[70];
  String File_Name = Folder_Name;
  String taskstring = "/Task1.txt";
  if(tasknum == 1){
   taskstring = "/Task1.txt"; 
  }
  else if(tasknum == 2){
   taskstring = "/Task2.txt"; 
  }
  else{
    taskstring = "/NoTask.txt";
  }
  
  File_Name.concat(taskstring);
  File_Name.toCharArray(File_Char, 70); 
  Serial.println(File_Char);
  if(SD_Initialized && Folder_Created){
     SD_Print_File = SD.open(File_Char, FILE_WRITE);
     if(SD_Print_File){
       File_Created = true;
       Serial.println("File Created");
       if(tasknum == 1){
        MakeHeader(ParticipantID, "Ring Tower Transfer", "Time Stamp", "Task Seq", "On Base", "Wire Touch", "Touch Time", "Total Touches"); 
       }
       else if(tasknum == 2){
        MakeHeader(ParticipantID, "Knot Tying", "Time Stamp", "Task Seq", "Eyelet Touch", "Pot Value", "Button Val", "Motor On"); 
       }
     }
  }
}

double timesec;
void PrintToFile(int taskseq, unsigned long time_stamp_mil, unsigned long input1, unsigned long input2, unsigned long input3, unsigned long input4){
  if(File_Created){
    timesec = (double) time_stamp_mil;
    timesec = floor(timesec*100.0) /100000.0;
    Serial.println(timesec);
    SD_Print_File.print(taskseq,DEC);
    SD_Print_File.print("\t");
    SD_Print_File.print(timesec,DEC);
    SD_Print_File.print("\t");
    SD_Print_File.print(input1,DEC);
    SD_Print_File.print("\t");
    SD_Print_File.print(input2,DEC);
    SD_Print_File.print("\t");
    SD_Print_File.print(input3,DEC);
    SD_Print_File.print("\t");
    SD_Print_File.print(input4,DEC);
    SD_Print_File.println();  // send a carriage return
  }
}

void MakeHeader(String ID, String TaskValue, String Column1, String Column2, String Column3, String Column4, String Column5, String Column6){
  if(File_Created){
    SD_Print_File.print("%Fundamentals of Robotic Surgery, Testee ID # ");
    SD_Print_File.print(ID);
    SD_Print_File.print(" Task: ");
    SD_Print_File.println(TaskValue);
    SD_Print_File.print("%");
    SD_Print_File.print(Column1);
    SD_Print_File.print("\t");
    SD_Print_File.print(Column2);
    SD_Print_File.print("\t");
    SD_Print_File.print(Column3);
    SD_Print_File.print("\t");
    SD_Print_File.print(Column4);
    SD_Print_File.print("\t");
    SD_Print_File.print(Column5);
    SD_Print_File.print("\t");
    SD_Print_File.print(Column6);
    SD_Print_File.println();  // send a carriage return
  }
}


