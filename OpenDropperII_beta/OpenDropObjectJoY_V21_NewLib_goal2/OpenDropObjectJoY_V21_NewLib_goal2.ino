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

Drop *myDrop1 = OpenDropDevice.getDrop();
Drop *myDrop2 = OpenDropDevice.getDrop();
Drop *myDrop3 = OpenDropDevice.getDrop();
Drop *myDrop4 = OpenDropDevice.getDrop();


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

myDrop1->begin(3,1);
myDrop2->begin(5,1);
myDrop3->begin(7,1);
myDrop4->begin(9,1);


  
  tone(SPK_pin,2000,100);
  delay(100);
  tone(SPK_pin,2000,100);
  delay(200);
  
  while(digitalRead(SW2_pin) == HIGH);
OpenDropDevice.update();


Serial.println("hello");



}

void blinken() {
        //       digitalWrite(LED_LIGHT_pin, digitalRead(LED_LIGHT_pin) ^ 1);
          //   digitalWrite(POL_pin, digitalRead(POL_pin) ^ 1);
           //  digitalWrite(BL_pin, digitalRead(BL_pin) ^ 1);

               }


// the loop function runs over and over again forever


void loop() {
  Serial.println("h");
  
int del_time=000;

myDrop1->begin(3,1);
myDrop2->begin(5,1);
myDrop3->begin(7,1);
myDrop4->begin(9,1);

myDrop1->go(3,1);
myDrop2->go(5,1);
myDrop3->go(7,1);
myDrop4->go(9,1);
 

while(OpenDropDevice.run())  delay(del_time);
 // OpenDropDevice.update();

myDrop1->go(3,1);
myDrop2->go(5,3);
myDrop3->go(7,5);
myDrop4->go(9,7);

while(OpenDropDevice.run())  delay(del_time);
 // OpenDropDevice.update();

myDrop1->go(1,1);
myDrop2->go(1,3);
myDrop3->go(1,5);
myDrop4->go(1,7);

while(OpenDropDevice.run())  delay(del_time);
 // OpenDropDevice.update();

myDrop1->go(3,1);
myDrop2->go(5,3);
myDrop3->go(7,5);
myDrop4->go(9,7);

while(OpenDropDevice.run())  delay(del_time);
 // OpenDropDevice.update();
 
myDrop1->go(3,7);
myDrop2->go(5,7);
myDrop3->go(7,7);
myDrop4->go(9,7);

while(OpenDropDevice.run())  delay(del_time);
 // OpenDropDevice.update();

myDrop1->go(3,7);
myDrop2->go(5,5);
myDrop3->go(7,3);
myDrop4->go(9,1);

while(OpenDropDevice.run())  delay(del_time);
 // OpenDropDevice.update();

myDrop1->go(1,7);
myDrop2->go(1,5);
myDrop3->go(1,3);
myDrop4->go(1,1);

 while(OpenDropDevice.run())  delay(del_time);
 // OpenDropDevice.update();

myDrop1->go(10,7);
myDrop2->go(10,3);
myDrop3->go(1,3);
myDrop4->go(1,1);

 while(OpenDropDevice.run())  delay(del_time);
 // OpenDropDevice.update();

myDrop1->go(10,5);
myDrop2->go(10,3);
myDrop3->go(6,5);
myDrop4->go(6,1);
 
  while(OpenDropDevice.run())  delay(del_time);
 // OpenDropDevice.update();

myDrop1->go(10,5);
myDrop2->go(10,3);
myDrop3->go(6,5);
myDrop4->go(6,3);
 
   while(OpenDropDevice.run())  delay(del_time);
 // OpenDropDevice.update();

myDrop1->go(9,5);
myDrop2->go(9,3);
myDrop3->go(7,5);
myDrop4->go(7,3);
 


//here

   while(OpenDropDevice.run())  delay(del_time);
 // OpenDropDevice.update();

myDrop1->go(9,5);
myDrop2->go(9,3);
myDrop3->go(3,5);
myDrop4->go(5,3);
 


   while(OpenDropDevice.run())  delay(del_time);
 // OpenDropDevice.update();

myDrop1->go(9,5);
myDrop2->go(7,3);
myDrop3->go(3,5);
myDrop4->go(5,3);
 
    while(OpenDropDevice.run())  delay(del_time);
 // OpenDropDevice.update();

myDrop1->go(9,1);
myDrop2->go(7,1);
myDrop3->go(3,1);
myDrop4->go(5,1);
 
     while(OpenDropDevice.run())  delay(del_time);
 
 
}






