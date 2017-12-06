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
//Drop *myDrop2 = OpenDropDevice.getDrop();



int JOY_value;
int joy_x,joy_y;

bool SWITCH_state=true;

  
// the setup function runs once when you press reset or power the board
void setup() {

Serial.begin(115200);

OpenDropDevice.begin();
OpenDropDevice.set_voltage(240,true,1000);
myDrop->begin(7,4);
//myDrop2->begin(1,1);


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

JOY_value = analogRead(JOY_pin);

  while((JOY_value>950)&&SWITCH_state)
  {
   JOY_value = analogRead(JOY_pin);
   SWITCH_state=digitalRead(SW1_pin);
  }




  Serial.print(JOY_value);


if  (JOY_value<300)
 { myDrop->move_right();Serial.println("right");}

if  ((JOY_value>730)&&(JOY_value<950))
 { myDrop->move_up();    Serial.println("up");
}
  
if  ((JOY_value>600)&&(JOY_value<730))
  {myDrop->move_left();Serial.println("left");}
  
if  ((JOY_value>450)&&(JOY_value<600))
  {myDrop->move_down();Serial.println("down");}

if (!SWITCH_state)
{ 
  Menu(OpenDropDevice);
  }


  
 OpenDropDevice.update_Drops();
  OpenDropDevice.update();

  delay(del_time);
  JOY_value = analogRead(JOY_pin);
   SWITCH_state=digitalRead(SW1_pin);



}






