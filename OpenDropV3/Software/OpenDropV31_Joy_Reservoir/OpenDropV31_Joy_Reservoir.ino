/*
  Basic Code to run the OpenDrop V3, Research platfrom for digital microfluidics
  Object codes are defined in the OpenDrop.h library
  Written by Urs Gaudenz from GaudiLabs
  2017
 */

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <OpenDrop.h>
#include <OpenDropAudio.h>

#include "hardware_def.h"


OpenDrop OpenDropDevice = OpenDrop(); 

Drop *myDrop = OpenDropDevice.getDrop();
Drop *myDrop2 = OpenDropDevice.getDrop();
Drop *myDrop3 = OpenDropDevice.getDrop();



int JOY_value;
int joy_x,joy_y;

bool SWITCH_state=true;
bool SWITCH_state2=true;

  
// the setup function runs once when you press reset or power the board
void setup() {

Serial.begin(115200);

OpenDropDevice.begin();
OpenDropDevice.set_voltage(240,false,1000);
myDrop->begin(7,4);
myDrop2->begin(15,4);
myDrop3->begin(15,4);

pinMode(JOY_pin, INPUT); 
                
OpenDropAudio.begin(16000);
OpenDropAudio.playMe(2);
 

  while(digitalRead(SW2_pin) == HIGH); //wait for key press
  
OpenDropDevice.update();
  Serial.println("Welcome to OpenDrop");
      
}

// the loop function runs over and over again forever


void loop() {
  
int del_time=100;
int del_counter=100;

JOY_value = analogRead(JOY_pin);

  while((JOY_value>950)&&SWITCH_state&&SWITCH_state2)
  {
   JOY_value = analogRead(JOY_pin);
   SWITCH_state=digitalRead(SW1_pin);
   SWITCH_state2=digitalRead(SW2_pin);
   del_counter--;
   if (del_counter<0) {
   OpenDropDevice.update_Display();
   del_counter=1000;
   }
  }


  Serial.print(JOY_value);


if  (JOY_value<256)
 { myDrop->move_right();Serial.println("right");}

if  ((JOY_value>725)&&(JOY_value<895))
 { myDrop->move_up();    Serial.println("up");
}
  
if  ((JOY_value>597)&&(JOY_value<725))
  {myDrop->move_left();Serial.println("left");}
  
if  ((JOY_value>256)&&(JOY_value<597))
  {myDrop->move_down();Serial.println("down");}

if (!SWITCH_state)
{ 
  OpenDropAudio.playMe(1);
  Menu(OpenDropDevice);
  }


if (!SWITCH_state2)
{ 

  myDrop->begin(15,5);
  myDrop2->begin(15,4);
  myDrop3->begin(15,6);
  OpenDropDevice.update_Drops();
  OpenDropDevice.update(); 
    delay(1000);  
    
  myDrop->begin(15,5);
  myDrop2->begin(15,6);
  myDrop3->begin(15,7);

  OpenDropDevice.update_Drops();
  OpenDropDevice.update();
  
    delay(1000);



    
  myDrop->begin(15,7);
  myDrop2->begin(15,4);
  myDrop3->begin(1,1);
  OpenDropDevice.update_Drops();
  OpenDropDevice.update(); 
    delay(1000);  

  myDrop->begin(14,6);
  myDrop2->begin(15,4);
  myDrop3->begin(1,1);
  OpenDropDevice.update_Drops();
  OpenDropDevice.update(); 
    delay(1000);    
  }

  
  OpenDropDevice.update_Drops();
  OpenDropDevice.update();

  delay(del_time);
  JOY_value = analogRead(JOY_pin);
   SWITCH_state=digitalRead(SW1_pin);
   SWITCH_state2=digitalRead(SW2_pin);



}






