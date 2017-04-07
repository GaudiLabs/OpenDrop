/*
  Basic Code to run the OpenDrop V2.1, Research platfrom for digital microfluidics
  Object codes are defined in the OpenDrop.h library
  Written by Urs Gaudenz from GaudiLabs
  2016
 */

#include <SPI.h>
#include <Wire.h>

#include "TimerOne.h"  
#include <avr/pgmspace.h>

#include "Adafruit_GFX.h"
#include <Adafruit_SSD1306.h>

#include <OpenDropII.h>
#include "hardware_def.h"




OpenDrop OpenDropDevice = OpenDrop(); 

Drop *myDrop = OpenDropDevice.getDrop();
Drop *myDrop2 = OpenDropDevice.getDrop();


boolean sound_flag=false;           // Digital Out Pin 2
boolean light_flag=true;           // Digital Out Pin 2
int JOY_value;
int FEEDBACK_value;




  
  
// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin 13 as an output.



const int AC_frequency=1000; // Frequency in Hz

  Timer1.initialize(500000/AC_frequency); 
  Timer1.attachInterrupt(blinken);
               
               
  Serial.begin(9600);
  

  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)




OpenDropDevice.begin();

myDrop->begin(6,3);
myDrop2->begin(1,1);



  
  tone(SPK_pin,2000,100);
  delay(100);
  tone(SPK_pin,2000,100);
  delay(200);
  
  while(digitalRead(SW2_pin) == HIGH);
OpenDropDevice.update();

myDrop2->go(13,7);



if (light_flag)   digitalWrite(LED_LIGHT_pin, HIGH);   // set SCK 
}

void blinken() {
        //       digitalWrite(LED_LIGHT_pin, digitalRead(LED_LIGHT_pin) ^ 1);
          //   digitalWrite(POL_pin, digitalRead(POL_pin) ^ 1);
           //  digitalWrite(BL_pin, digitalRead(BL_pin) ^ 1);

               }


// the loop function runs over and over again forever


void loop() {
  


  
int del_time=100;

JOY_value = analogRead(JOY_pin);

  while(JOY_value>950){
   JOY_value = analogRead(JOY_pin);
     }





  //Serial.print(JOY_value);


if  (JOY_value<300)
  myDrop->move_right();

if  ((JOY_value>700)&&(JOY_value<950))
  myDrop->move_up();
  
if  ((JOY_value>600)&&(JOY_value<700))
  myDrop->move_left();
  
if  ((JOY_value>450)&&(JOY_value<600))
  myDrop->move_down();

 //OpenDropDevice.run();
  OpenDropDevice.update();

  delay(del_time);
  JOY_value = analogRead(JOY_pin);



}






