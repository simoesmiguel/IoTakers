/* 
  * Complete Guide for Ultrasonic Sensor HC-SR04
*/
// include the library code
#include <ArduinoJson.h>
#include <QList.h>
#include <TFT.h>  // Arduino LCD library
#include <SPI.h>

// pin definition for the Uno
#define cs   10
#define dc   9
#define rst  8



// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to

// create an instance of the library
TFT TFTscreen = TFT(cs, dc, rst);



//ultraSound sensor pins
int trigPin = 5;    // Trigger
int echoPin = 4;    // Echo
int trigPin2 = 6;    // Trigger 2
int echoPin2 = 7;    // Echo 2 

int led1 = 2;
int led2 = 3;

int const pot=12;

int previous_value1=-1;
int previous_value2=-1;
int treshold=17;
float t,t2;
boolean empty_1,empty_2;
long waiting_time=30000;

float duration, cm;
unsigned long sending_delay;
boolean blinkk=false;

int days1=0;
int days2=0;
int prevValuePot;
QList<int> distances;


void setup() {
    // Put this line at the beginning of every sketch that uses the GLCD:
  TFTscreen.begin();

  // clear the screen with a black background
  TFTscreen.background(0, 0, 0);

  // write the static text to the screen
  // set the font color to white
  TFTscreen.stroke(0, 255, 0);
  // set the font size
  TFTscreen.setTextSize(2);
  // write the text to the top left corner of the screen
  TFTscreen.text("IoTakers ", 0, 0);
  //TFTscreen.setTextSize(2);
  TFTscreen.stroke(255, 255, 255);
  TFTscreen.text("Previsao", 35, 30);
  TFTscreen.text("P1:", 0, 60);
  TFTscreen.text("P2:", 0, 90);
  TFTscreen.text(" dias", 95, 60);
  TFTscreen.text(" dias", 95, 90);
  
  //Serial Port begin
  Serial.begin (9600);
  //Define inputs and outputs
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);


  pinMode (led2, OUTPUT); //2
  pinMode (led1, OUTPUT); //1
  
  sending_delay=millis(); //start the delay counter

  float cm1= readSensorsInformation(1);
  float cm2=readSensorsInformation(2);
  distances.push_back(cm1);
  distances.push_back(cm2);
  checkDistance(cm1,cm2);
  showInformation();
  days1= convertDistDays(cm1);
  days2=convertDistDays(cm2);
  sendMessage(String(days1),String(days2));
  
 // prevValuePot= readPotentiometer();

}



void loop() {
  checkPotentiometer();
  if(blinkk){
    digitalWrite(led2,HIGH);
    delay(500);
    digitalWrite(led2,LOW);
    delay(500);
  }
  float cm1= readSensorsInformation(1);
  float cm2= readSensorsInformation(2);
  checkDistance(cm1,cm2);
  days1= convertDistDays(cm1);
  days2=convertDistDays(cm2);
  showInformation();
  /*
  Serial.println((int)cm1);
  Serial.println((int)cm2);
  Serial.print("distances at 0: ");
  Serial.println(distances.at(0));
  Serial.print("distances at 1: ");
  Serial.println(distances.at(1));*/
  
  bool vary=false;
  if(((int)cm1)!=distances.at(0)){
    //houve variacao na caixa 1
    distances.pop_front();
    distances.push_front(cm1);
    vary=true;
  }
  if(((int)cm2)!=distances.at(1)){
    //houve variacao na caixa 2
    distances.pop_back();
    distances.push_back(cm2);    
    vary=true;
  }
  if(vary){
    days1= convertDistDays(cm1);
    days2=convertDistDays(cm2);
    sendMessage(String(days1),String(days2));
  }
  

}

void sendMessage(String d1,String d2){

  String json= "&{\"P1\":\""+d1+"\",\"P2\":\""+d2+"}%"; 
  
  /*
  if(empty_1==true && empty_2==true){
    if(((millis()-t2)> waiting_time)&&((millis()-t)> waiting_time)){
      // json = "&{\"medicineName\":\"P1_P2\",\"stock_days\":2_1}%"; 
    }
  }
  
  else if(empty_2==true){
    if((millis()-t2)> waiting_time){
      // json = "&{\"medicineName\":\"P2\",\"stock_days\":2}%"; 
    }
  }
  else if(empty_1==true){
    if((millis()-t)> waiting_time){
      // json = "&{\"medicineName\":\"P1\",\"stock_days\":4}%"; 
    }
  }*/
  Serial.println(json);
}



float readSensorsInformation(int sensorId){
  duration=0;
  if(sensorId==1){
    digitalWrite(trigPin, LOW);
    delayMicroseconds(5);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    
    duration = pulseIn(echoPin, HIGH);
    
  }else{
    digitalWrite(trigPin2, LOW);
    delayMicroseconds(5);
    digitalWrite(trigPin2, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin2, LOW);
    
    duration = pulseIn(echoPin2, HIGH);
    
  }
 
  // Convert the time into a distance
  cm = (duration/2) / 29.1;     // Divide by 29.1 or multiply by 0.0343
  //Serial.print("sensorID:" );
  //Serial.println(sensorId);
  Serial.print("Cm: ");
  Serial.println(cm);
  /*
  Serial.print("cm");
  Serial.println();
  delay(250);*/
  return cm;
}

void showInformation(){
 
  // print the number of seconds since reset:
  //lcd.print(millis() / 1000);
  String toPrint = " ";
  
  if(empty_1==true && empty_2==true){
    if(((millis()-t2)> waiting_time)&&((millis()-t)> waiting_time)){
      
      toPrint ="P1 e P2 vazias";
      turnLedsOn("1");
      turnLedsOn("2");
    }    
  }
  else if(empty_2==true){
    if((millis()-t2)> waiting_time){
        
        
        turnLedsOn("2");
      }
  }
  else if(empty_1==true){
    if((millis()-t)> waiting_time){
      toPrint ="P1 esta vazia";
      turnLedsOn("1");
    }
  }
  if(empty_1 ==false)
    turnLedsOff("1");
  if(empty_2 ==false)
    turnLedsOff("2");
  if(empty_1 ==false && empty_2 ==false)
  {  
    turnLedsOff("1");    
    turnLedsOff("2");
  }
  if(previous_value2=="??" && days2!= 0){
    blinkk=false;
  }
   if(blinkk){
   //P1 state
      int buffersize = String("??").length()+1;
      char textBuffer[buffersize];
      TFTscreen.stroke(0, 0,0);
      TFTscreen.fill(0,0,0);
      TFTscreen.rect(40, 90, 50, 15 );
      TFTscreen.stroke(255, 255, 255);
    
      String("??").toCharArray(textBuffer,buffersize);
      TFTscreen.text(textBuffer,40,90);  //(159,98)max  coordinates
      previous_value2="??";
  
    }
  else{
    //P2 state
     if(days2!=previous_value2){
  
      int buffersize2 = String(days2).length()+1;
      char textBuffer2[buffersize2];
       TFTscreen.stroke(0, 0,0);
       TFTscreen.fill(0,0,0);
       TFTscreen.rect(40, 90, 50, 15 );
       TFTscreen.stroke(255, 255, 255);
     
        String(days2).toCharArray(textBuffer2,buffersize2);
        TFTscreen.text(textBuffer2,40,90);  //(159,98)max  coordinates
        previous_value2=days2;
      }
    }
  
    if(days1!=previous_value1){
   //P1 state
      int buffersize = String(days1).length()+1;
      char textBuffer[buffersize];
      TFTscreen.stroke(0, 0,0);
      TFTscreen.fill(0,0,0);
      TFTscreen.rect(40, 60, 50, 15 );
      TFTscreen.stroke(255, 255, 255);
    
      String(days1).toCharArray(textBuffer,buffersize);
      TFTscreen.text(textBuffer,40,60);  //(159,98)max  coordinates
      previous_value1=days1;
  
    }
  
   
  

}

void checkDistance(float cm1,float cm2){
  if(cm1>treshold ){
    if(empty_1==false){
      t=millis(); //start the time counter
      empty_1=true;
    }
  }
  else
    empty_1=false;
   
  if(cm2>treshold){
    if(empty_2==false){
      t2=millis();
      empty_2=true;
    }
  }
  else
    empty_2=false;

}


void turnLedsOn(String led){
  if(led=="1")
       digitalWrite(led1, HIGH);
  if(led=="2")
       digitalWrite(led2, HIGH);

}

void turnLedsOff(String led){
  if(led=="1")
       digitalWrite(led1, LOW);
  if(led=="2")
       digitalWrite(led2, LOW);
}



// returns the stock (in days) given a medicine Id
int getDays(int medicineId){
  
}

void startDelayCounter(){
  sending_delay=millis();
}

bool checkDelay(){
  if(millis()-sending_delay>15000){ //a enviar de 15 em 15 seg
    //reset the delay counter 
    sending_delay = millis();      
    return true;
  }
  return false;
}


int convertDistDays(float dist){
  QList<int> days_list;
  days_list.push_back(1);
  days_list.push_back(2);
  days_list.push_back(4);
  days_list.push_back(5);
  days_list.push_back(6);
  days_list.push_back(10);
  days_list.push_back(11);
  days_list.push_back(12);
  days_list.push_back(13);
  days_list.push_back(17);
  days_list.push_back(18);
  days_list.push_back(19);
  days_list.push_back(21);
  days_list.push_back(23);
  days_list.push_back(30);

  int d=round(30/round(dist));
  int days=0;

  if(round(dist)==21 ||round(dist)==22||round(dist)==20)
    days=0;
  else if(d<=15)
    days = days_list.at(d);
  else if(d>15)
    days = 20;
  String string_days= String(days); 
  if(string_days.length()==1)
    string_days = "0"+string_days;    
  return atoi(string_days.c_str()); 
}

void checkPotentiometer(){
  if(readPotentiometer()!= prevValuePot){
    blinkk=true; 
    prevValuePot=readPotentiometer();
  }
}

int readPotentiometer(){
  return digitalRead(pot); 
}
