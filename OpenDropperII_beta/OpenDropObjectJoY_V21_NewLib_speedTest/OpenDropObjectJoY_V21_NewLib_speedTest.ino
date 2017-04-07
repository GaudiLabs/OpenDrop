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



int JOY_value;
int FEEDBACK_value;


int del_time=100;


  
  
// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin 13 as an output.



const int AC_frequency=1000; // Frequency in Hz

  Timer1.initialize(500000/AC_frequency); 
  Timer1.attachInterrupt(blinken);
               
               
  Serial.begin(9600);
  

  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)




OpenDropDevice.begin();

myDrop1->begin(5,1);
myDrop2->begin(7,3);
myDrop3->begin(13,1);




  
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
  


if (digitalRead(SW2_pin) == HIGH) {del_time=del_time-5;}
if (digitalRead(SW1_pin) == HIGH) {del_time=del_time+5;}


myDrop1->move_right();
myDrop2->move_down();
myDrop3->move_down();

OpenDropDevice.update_Drops();
OpenDropDevice.update();

delay (del_time);

myDrop1->move_right();
myDrop2->move_right();
myDrop3->move_down();


OpenDropDevice.update_Drops();
OpenDropDevice.update();
delay (del_time);

myDrop1->move_right();
myDrop2->move_up();
myDrop3->move_down();


OpenDropDevice.update_Drops();
OpenDropDevice.update();
delay (del_time);

myDrop1->move_right();
myDrop2->move_left();
myDrop3->move_down();


OpenDropDevice.update_Drops();
OpenDropDevice.update();
delay (del_time);

myDrop1->move_right();
myDrop2->move_down();
myDrop3->move_down();


OpenDropDevice.update_Drops();
OpenDropDevice.update();
delay (del_time);

/////

myDrop1->move_down();
myDrop2->move_right();
myDrop3->move_up();

OpenDropDevice.update_Drops();
OpenDropDevice.update();
delay (del_time);

myDrop1->move_down();
myDrop2->move_up();
myDrop3->move_up();


OpenDropDevice.update_Drops();
OpenDropDevice.update();
delay (del_time);

myDrop1->move_down();
myDrop2->move_left();
myDrop3->move_up();


OpenDropDevice.update_Drops();
OpenDropDevice.update();
delay (del_time);

myDrop1->move_down();
myDrop2->move_down();
myDrop3->move_up();


OpenDropDevice.update_Drops();
OpenDropDevice.update();
delay (del_time);

myDrop1->move_down();
myDrop2->move_right();
myDrop3->move_up();


OpenDropDevice.update_Drops();
OpenDropDevice.update();
delay (del_time);

/////

myDrop1->move_left();
myDrop2->move_up();
myDrop3->move_down();

OpenDropDevice.update_Drops();
OpenDropDevice.update();
delay (del_time);

myDrop1->move_left();
myDrop2->move_left();
myDrop3->move_down();

OpenDropDevice.update_Drops();
OpenDropDevice.update();
delay (del_time);

myDrop1->move_left();
myDrop2->move_down();
myDrop3->move_down();

OpenDropDevice.update_Drops();
OpenDropDevice.update();delay (del_time);

myDrop1->move_left();
myDrop2->move_right();
myDrop3->move_down();

OpenDropDevice.update_Drops();
OpenDropDevice.update();delay (del_time);

myDrop1->move_left();
myDrop2->move_up();
myDrop3->move_down();

OpenDropDevice.update_Drops();
OpenDropDevice.update();delay (del_time);

/////

myDrop1->move_up();
myDrop2->move_left();
myDrop3->move_up();

OpenDropDevice.update_Drops();
OpenDropDevice.update();delay (del_time);

myDrop1->move_up();
myDrop2->move_down();
myDrop3->move_up();

OpenDropDevice.update_Drops();
OpenDropDevice.update();delay (del_time);

myDrop1->move_up();
myDrop2->move_right();
myDrop3->move_up();

OpenDropDevice.update_Drops();
OpenDropDevice.update();delay (del_time);

myDrop1->move_up();
myDrop2->move_up();
myDrop3->move_up();

OpenDropDevice.update_Drops();
OpenDropDevice.update();delay (del_time);

myDrop1->move_up();
myDrop2->move_left();
myDrop3->move_up();

OpenDropDevice.update_Drops();
OpenDropDevice.update();delay (del_time);

/////


 
}






