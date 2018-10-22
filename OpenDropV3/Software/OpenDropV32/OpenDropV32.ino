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


bool FluxCom[16][8];


int JOY_value;
int joy_x,joy_y;
int x,y;
int del_counter=0;
int del_counter2=0;

bool SWITCH_state=true;
bool SWITCH_state2=true;
int j=0; 
  
// the setup function runs once when you press reset or power the board
void setup() {

Serial.begin(115200);

OpenDropDevice.begin();
OpenDropDevice.set_voltage(240,false,1000);

OpenDropDevice.set_Fluxels(FluxCom);



pinMode(JOY_pin, INPUT);  
                
OpenDropAudio.begin(16000);
OpenDropAudio.playMe(2);
delay (2000);
 
OpenDropDevice.drive_Fluxels();
OpenDropDevice.update_Display();
  Serial.println("Welcome to OpenDrop");

myDrop->begin(7,4);
OpenDropDevice.update();

}

// the loop function runs over and over again forever


void loop() {
    if (Serial.available()>0) 
{
    if (Serial.read()==1) FluxCom[x][y] =true; else FluxCom[x][y] =false;
    y++;
    digitalWrite(LED_Rx_pin,HIGH); 
    if (y==FluxlPad_heigth) 
    {y=0;
    
    x++;
    if (x==FluxlPad_width)
    {
      OpenDropDevice.set_Fluxels(FluxCom);
OpenDropDevice.drive_Fluxels();
OpenDropDevice.update_Display();
x=0;
 //OpenDropAudio.playMe(1);
 
      };
    }


} else digitalWrite(LED_Rx_pin,LOW); 

   del_counter--;

 if (del_counter<0) {
   OpenDropDevice.update_Display();
   del_counter=1000;
   }

SWITCH_state=digitalRead(SW1_pin);

if (!SWITCH_state)
{ 
  OpenDropAudio.playMe(1);
  Menu(OpenDropDevice);
  OpenDropDevice.update_Display();
del_counter2=200;

  }


JOY_value = analogRead(JOY_pin);

if ((JOY_value<950)&(del_counter2==0))
{
if  (JOY_value<256)
 { myDrop->move_right();Serial.println("right");
}

if  ((JOY_value>725)&&(JOY_value<895))
 { myDrop->move_up();    Serial.println("up");

}
  
if  ((JOY_value>597)&&(JOY_value<725))
  {myDrop->move_left();Serial.println("left");
}
  
if  ((JOY_value>256)&&(JOY_value<597))
  {myDrop->move_down();Serial.println("down");
}

  OpenDropDevice.update_Drops();
    OpenDropDevice.update(); 
del_counter2=200;
del_counter=1000;
} 

if (JOY_value>950) del_counter2=0;
if (del_counter2>0) del_counter2--;

}






