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
int joy_x,joy_y=0;

bool SWITCH_state=true;

  
// the setup function runs once when you press reset or power the board
void setup() {

Serial.begin(115200);

OpenDropDevice.begin();
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
  
int del_time=60;

JOY_value = analogRead(JOY_pin);

  while((JOY_value>950)&(digitalRead(SW3_pin)))
   JOY_value = analogRead(JOY_pin);

if (!digitalRead(SW3_pin)) {
  myDrop->go(joy_x,joy_y);
  while(OpenDropDevice.run())  delay(del_time);
    Serial.println("llll");
}


  
if  (JOY_value<300)
 joy_x++;

if  ((JOY_value>700)&&(JOY_value<950))
 joy_y=joy_y-1;
  
if  ((JOY_value>600)&&(JOY_value<700))
 joy_x=joy_x-1;
  
if  ((JOY_value>450)&&(JOY_value<600))
 joy_y++;
  
OpenDropDevice.set_joy(joy_x,joy_y);
OpenDropDevice.update();  

 
 delay(del_time);
/*
myDrop->go(2,2);
//myDrop2->go(13,6);

while(OpenDropDevice.run())  delay(del_time);
 // OpenDropDevice.update();

myDrop->go(13,3);
//myDrop2->go(2,2);

while(OpenDropDevice.run())  delay(del_time);

myDrop->go(12,6);
//myDrop2->go(7,7);

while(OpenDropDevice.run())  delay(del_time);

myDrop->go(2,7);
//myDrop2->go(7,7);

while(OpenDropDevice.run())  delay(del_time);
*/


}



