/*
  Basic Code to run the OpenDrop V2.1, Research platfrom for digital microfluidics
  Object codes are defined in the OpenDrop.h library
  Written by Urs Gaudenz from GaudiLabs
  2016
 */

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <OpenDrop.h>
#include "hardware_def.h"


// If using software SPI (the default case):
#define OLED_MOSI   2
#define OLED_CLK   3
#define OLED_DC    11
#define OLED_CS    12
#define OLED_RESET 13
Adafruit_SSD1306 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

#define BL_pin         9           // BL Pin
#define LE_pin          4           // LE Pin
#define CLK_pin         15           // CLK Pin
#define DI_pin         16           // Digital Out Pin

#define SW1_pin         8           // Buton 1 pin
#define SW2_pin         A5           // Buton 1 pin
#define SW3_pin         7           // Buton 1 pin
#define JOY_pin         A4           // Buton 1 pin
#define FEEDBACK_pin    A2           // Buton 1 pin


#define ESP_RESET_pin   A3           // Buton 1 pin
#define ESP_GPIO0_pin   10           // Buton 1 pin
#define ESP_RX_pin      1           // Buton 1 pin
#define ESP_TX_pin      0           // Buton 1 pin


#define LED_LIGHT_pin         5           // Digital  Pin for LEDs
#define SPK_pin       6           // Digital Pin for speaker

boolean sound_flag=true;           // Digital Out Pin 2
boolean light_flag=true;           // Digital Out Pin 2
int JOY_value;
int FEEDBACK_value;

  Drop drop[] = {Drop(1),Drop(1),Drop(1)};
  bool Fluxls[FluxlPad_width][FluxlPad_heigth];
  
  
// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin 13 as an output.


  pinMode(BL_pin, OUTPUT); //BL pin
  digitalWrite(BL_pin, HIGH);   // set BL 
  pinMode(LE_pin, OUTPUT); //LE pin
  digitalWrite(LE_pin, LOW);   // set LE 
  pinMode(CLK_pin, OUTPUT); //SCK pin
  digitalWrite(CLK_pin, LOW);   // set SCK 
  pinMode(DI_pin, OUTPUT); //DI pin
  digitalWrite(DI_pin, LOW);   // DI LE 

  pinMode(LED_LIGHT_pin, OUTPUT); //SCK pin
  
  pinMode(SW1_pin, INPUT); 
  digitalWrite(SW1_pin, HIGH);  
  pinMode(SW2_pin, INPUT); 
  digitalWrite(SW2_pin, HIGH);  
  pinMode(SW3_pin, INPUT); 
  digitalWrite(SW3_pin, HIGH);  
  pinMode(JOY_pin, INPUT); 

  Serial.begin(9600);

  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC);
  // init done
  
  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
  display.display();
  delay(2000);

  // Clear the buffer.
  display.clearDisplay();
display.dim(false);

display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(5,20);
  display.println("Loading protocol...");
  display.display();
  delay(500);
  display.setCursor(5,29);
  display.println("Press Start");
display.display();


  drop[0].begin(6,3);
  drop[1].begin(1,1);
  drop[2].begin(1,1);
  
    update();

  
  tone(SPK_pin,2000,100);
  delay(100);
  tone(SPK_pin,2000,100);
  delay(200);
  while(digitalRead(SW2_pin) == HIGH);



if (light_flag)   digitalWrite(LED_LIGHT_pin, HIGH);   // set SCK 
}


// the loop function runs over and over again forever
void loop() {
  
int del_time=100;

JOY_value = analogRead(JOY_pin);

  while(JOY_value>950)
   JOY_value = analogRead(JOY_pin);


  Serial.print(JOY_value);


if  (JOY_value<300)
  drop[0].move_right();

if  ((JOY_value>700)&&(JOY_value<950))
  drop[0].move_up();
  
if  ((JOY_value>600)&&(JOY_value<700))
  drop[0].move_left();
  
if  ((JOY_value>450)&&(JOY_value<600))
  drop[0].move_down();

  update();
  delay(del_time);
  JOY_value = analogRead(JOY_pin);

}



void update()
{
  
const byte pad_lookup_x [64] PROGMEM = {
0,0,0,0,1,1,1,1,
2,2,2,2,3,3,3,3,
4,4,4,4,5,5,5,5,
6,7,7,6,6,7,7,6,
6,7,7,6,7,6,7,6,
5,5,5,5,4,4,4,4,
3,3,3,3,2,2,2,2,
1,1,1,1,0,0,0,0
};

const byte pad_lookup_y [64] PROGMEM = {
3,2,1,0,3,2,0,1,
3,1,0,2,3,0,1,2,
3,0,1,2,0,1,2,3,
0,1,2,2,1,0,3,3,
4,4,7,6,5,5,6,7,
4,5,6,7,5,6,7,4,
5,6,7,4,5,7,6,4,
6,7,5,4,7,6,5,4
};




clear_Fluxels();                     //clear Fluxel Array


//Fill Fluxel Array
for (int i = 0; i <3 ; i++) 
{
  Fluxls[drop[i].position_x()][drop[i].position_y()]=true;
};


// clear Display
  display.clearDisplay();
// write coordinates
 display.setCursor(18,4);
  display.print("X: ");
  display.print(drop[0].position_x());
  display.print(" Y: ");
  display.print(drop[0].position_y());

// draw Grid
  for (int x = 1; x <FluxlPad_width ; x++) 
    for (int y = 1; y <FluxlPad_heigth ; y++) 
  display.drawPixel(x*6+16,y*6+16,WHITE);
  
  
// draw Fluxels
      for (int x = 0; x <FluxlPad_width ; x++) 
    for (int y = 0; y <FluxlPad_heigth ; y++) 
   if (Fluxls[x][y]) {
           display.fillRect(x*6+17, y*6+17, 5,5, 1);
    }
     
// draw Rect
     display.drawRect(16, 16, 97,48, 1);

// update display
    display.display();
   
   

// Fill chip

    digitalWrite(LE_pin, LOW);   // set LE 
    digitalWrite(CLK_pin, LOW);   // set SCK 
   
    for (int i = 0; i <64 ; i++) 

{
   digitalWrite(DI_pin,Fluxls[pad_lookup_x[i]][pad_lookup_y[i]]);

   digitalWrite(CLK_pin, HIGH);   // set LE
   digitalWrite(CLK_pin, LOW);   // set LE
 
  //Serial.println(Fluxls[pad_lookup_x[i]][pad_lookup_y[i]]);
   };
   
     for (int i = 0; i <64 ; i++) 
{
   digitalWrite(DI_pin,Fluxls[15-pad_lookup_x[i]][7-pad_lookup_y[i]]);

   digitalWrite(CLK_pin, HIGH);   // set LE
   digitalWrite(CLK_pin, LOW);   // set LE
 
  //Serial.println(Fluxls[pad_lookup_x[i]][pad_lookup_y[i]]);
   };
   
   
  
    digitalWrite(LE_pin, HIGH);   // set LE 
    digitalWrite(LE_pin, LOW);   // set LE 
    
FEEDBACK_value=analogRead(FEEDBACK_pin);

    
 if(sound_flag)   tone(SPK_pin,FEEDBACK_value*2,50);  
  
};


void clear_Fluxels()
{
    for (int x = 0; x <FluxlPad_width ; x++) 
    for (int y = 0; y <FluxlPad_heigth ; y++) 
  Fluxls[x][y]=false;

};
