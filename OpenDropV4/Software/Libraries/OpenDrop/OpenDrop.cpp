/*--------------------------------------------------------------------
  This file is part of the OpenDrop library
  by Urs Gaudenz, GaudiLabs 2016
  --------------------------------------------------------------------*/



#include "OpenDrop.h"
#include "hardware_def.h"
#include "bitmap.h"
#include "Adafruit_GFX.h"
#include <Adafruit_SSD1306.h>
#include <OpenDropAudio.h>


  bool Fluxls[FluxlPad_width][FluxlPad_heigth];
  byte pad_feedback [128];

uint32_t AC_frequency=1000; // Frequency in Hz
uint16_t Voltage_set=240; // Voltage in Volt

boolean AC_flag=false;     // AC or DC voltage selection
boolean HV_enable=false;     // AC or DC voltage selection
bool state = 0; //just for an example

float VOLTAGE_value;
long VOLTAGE_set_value;

bool sound=sound_flag;
// used for SPI bitbang
byte cmd_byte0 = B00010001; // command byte to write to pot 0, from the MCP42XXX datasheet
byte work = B00000000; // setup a working byte, used to bit shift the data out

char version_str[] ="V4.01";


Adafruit_SSD1306 display( OLED_MOSI,OLED_CLK,OLED_DC, OLED_RESET, OLED_CS);

//********** Interupt Functions **********


//this function gets called by the interrupt at <sampleRate>Hertz
void TC4_Handler (void) {
 if ((AC_flag==true)&&(HV_enable==true)){
  if(state == true) {
      digitalWrite(AC_pin, LOW);
      delayMicroseconds(75); //67
      digitalWrite(POL_pin, INVERTED2);
   // digitalWrite(LED_D13_pin,LOW);
  } else {
      digitalWrite(AC_pin, HIGH);
      delayMicroseconds(81); 
      digitalWrite(POL_pin, !INVERTED2);
   // digitalWrite(LED_D13_pin,HIGH);
  }

 } else 

 {
   digitalWrite(AC_pin, HIGH);
   digitalWrite(POL_pin, !HV_enable);
  
  }

  
  state = !state;
  // END OF YOUR CODE
  TC4->COUNT16.INTFLAG.bit.MC0 = 1; //don't change this, it's part of the timer code
}




// ******** Functions *****************

/* code by sellensq */

void pinStr( uint32_t ulPin, unsigned strength) // works like pinMode(), but to set drive strength
{
  // Handle the case the pin isn't usable as PIO
  if ( g_APinDescription[ulPin].ulPinType == PIO_NOT_A_PIN )
  {
    return ;
  }
  if(strength) strength = 1;      // set drive strength to either 0 or 1 copied
  PORT->Group[g_APinDescription[ulPin].ulPort].PINCFG[g_APinDescription[ulPin].ulPin].bit.DRVSTR = strength ;
}




/* 
 *  TIMER SPECIFIC FUNCTIONS FOLLOW
 *  you shouldn't change these unless you know what you're doing
 */

//Configures the TC to generate output events at the sample frequency.
//Configures the TC in Frequency Generation mode, with an event output once
//each time the audio sample frequency period expires.
//Reset TC4 

//Function that is used to check if TC4 is done syncing
//returns true when it is done syncing
bool tcIsSyncing()
{
  return TC4->COUNT16.STATUS.reg & TC_STATUS_SYNCBUSY;
}

void tcReset()
{
  TC4->COUNT16.CTRLA.reg = TC_CTRLA_SWRST;
  while (tcIsSyncing());
  while (TC4->COUNT16.CTRLA.bit.SWRST);
}

 
void tcConfigure(int sampleRate)
{
 // Enable GCLK for TCC2 and TC4 (timer counter input clock)
 GCLK->CLKCTRL.reg = (uint16_t) (GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_ID(GCM_TC4_TC5)) ;
 while (GCLK->STATUS.bit.SYNCBUSY);

 tcReset(); //reset TC4

 // Set Timer counter Mode to 16 bits
 TC4->COUNT16.CTRLA.reg |= TC_CTRLA_MODE_COUNT16;
 // Set TC4 mode as match frequency
 TC4->COUNT16.CTRLA.reg |= TC_CTRLA_WAVEGEN_MFRQ;
 //set prescaler and enable TC4
 TC4->COUNT16.CTRLA.reg |= TC_CTRLA_PRESCALER_DIV8 | TC_CTRLA_ENABLE;
 //set TC4 timer counter based off of the system clock and the user defined sample rate or waveform
 TC4->COUNT16.CC[0].reg = (uint16_t) (SystemCoreClock /16/ sampleRate - 1);
 while (tcIsSyncing());
 
 // Configure interrupt request
 NVIC_DisableIRQ(TC4_IRQn);
 NVIC_ClearPendingIRQ(TC4_IRQn);
 NVIC_SetPriority(TC4_IRQn, 0);
 NVIC_EnableIRQ(TC4_IRQn);

 // Enable the TC4 interrupt request
 TC4->COUNT16.INTENSET.bit.MC0 = 1;
 while (tcIsSyncing()); //wait until TC4 is done syncing 
} 


// function enables TC4 and waits for it to be ready
void tcStartCounter()
{
  TC4->COUNT16.CTRLA.reg |= TC_CTRLA_ENABLE; //set the CTRLA register
  while (tcIsSyncing()); //wait until snyc'd
}



//disable TC4
void tcDisable()
{
  TC4->COUNT16.CTRLA.reg &= ~TC_CTRLA_ENABLE;
  while (tcIsSyncing());
}




/* SPI by Hand

by Sebastian Tomczak

20 July 2007

Adelaide, Australia

*/

void spi_transfer(byte working) {

; // function to actually bit shift the data byte out

for(int i = 1; i <= 8; i++) { // setup a loop of 8 iterations, one for each bit

if (working > 127) { // test the most significant bit

digitalWrite (VSI_pin,HIGH); // if it is a 1 (ie. B1XXXXXXX), set the master out pin high

}

else {

digitalWrite (VSI_pin, LOW); // if it is not 1 (ie. B0XXXXXXX), set the master out pin low

}

digitalWrite (VSCK_pin,HIGH); // set clock high, the pot IC will read the bit into its register

working = working << 1;

digitalWrite(VSCK_pin,LOW); // set clock low, the pot IC will stop reading and prepare for the next iteration (next significant bit

}

}

void spi_out(int SS, byte cmd_byte, byte data_byte) { // SPI tranfer out function begins here

digitalWrite (SS, LOW); // set slave select low for a certain chip, defined in the argument in the main loop. selects the chip

work = cmd_byte; // let the work byte equal the cmd_byte, defined in the argument in the main loop

spi_transfer(work); // transfer the work byte, which is equal to the cmd_byte, out using spi

work = data_byte; // let the work byte equal the data for the pot

spi_transfer(work); // transfer the work byte, which is equal to the data for the pot

digitalWrite(SS, HIGH); // set slave select high for a certain chip, defined in the argument in the main loop. deselcts the chip

}



void clear_Fluxels()
{
    for (int x = 0; x <FluxlPad_width ; x++) 
    for (int y = 0; y <FluxlPad_heigth ; y++) 
  Fluxls[x][y]=false;

};


bool free_Fluxel(uint8_t  x,uint8_t  y,uint8_t  dir)
{bool check=true;


if (check&&(x>0)&&(dir==4)) check = pad_feedback[pgm_read_byte_near(&FluxelID[x-1][y])]==1; //links
if (check&&(x<(FluxlPad_width-1))&&(dir==2)) check = pad_feedback[pgm_read_byte_near(&FluxelID[x+1][y])]==1; //rechts
if (check&&(y>0)&&(dir==1)) check = pad_feedback[pgm_read_byte_near(&FluxelID[x][y-1])]==1; // oben
if (check&&(y<(FluxlPad_heigth-1))&&(dir==3)) check = pad_feedback[pgm_read_byte_near(&FluxelID[x][y+1])]==1; //unten
if (check&&(x>0)&(y>0)&&((dir==1)||(dir==4))) check = pad_feedback[pgm_read_byte_near(&FluxelID[x-1][y-1])]==1; //links oben
if (check&&(x>0)&(y<(FluxlPad_heigth-1))&&((dir==3)||(dir==4))) check = pad_feedback[pgm_read_byte_near(&FluxelID[x-1][y+1])]==1; //links unten
if (check&&(x<(FluxlPad_width-1))&&(y>0)&&((dir==1)||(dir==2))) check = pad_feedback[pgm_read_byte_near(&FluxelID[x+1][y-1])]==1; //rechts oben
if (check&&(x<(FluxlPad_width-1))&&(y<(FluxlPad_heigth-1))&&((dir==2)||(dir==3))) check = pad_feedback[pgm_read_byte_near(&FluxelID[x+1][y+1])]==1; //rechts unten


return check;
};








// *************** Definitions ****************

OpenDrop::OpenDrop(uint8_t addr) {
  _addr = addr;

}

void OpenDrop::update_Display(void) // Updated Display with Fluxls data
   
{
// clear Display
  display.clearDisplay();  


// write coordinates
  display.setCursor(14,4);
  display.print("X: ");
  display.print(drops[0].position_x());
  display.print(" Y: ");
  display.print(drops[0].position_y());
  display.print(" ");
  VOLTAGE_value=0.5*VOLTAGE_value+0.5*(analogRead(VSENS_pin)*3.3*(1500000+6800+5000)/1024/6800);
  if (AC_flag) display.print("AC "); else display.print("DC ");
  display.print((int)VOLTAGE_value);
  display.print("V");

// draw Grid
  for (int x = 1; x <(FluxlPad_width-1) ; x++) 
    for (int y = 1; y <FluxlPad_heigth ; y++) 
  display.drawPixel(x*6+16,y*6+15,WHITE);
  
  
// draw Fluxels and Reservoirs
      for (int x = 0; x <(FluxlPad_width) ; x++) 
    for (int y = 0; y <FluxlPad_heigth ; y++) 
   if (Fluxls[x][y]) {
           if (x==0)
 	   {if (y==0) display.drawRect(x*6+17, 6+16, 5,5, 1);							          // Entrance
            if (y==1) {display.drawRect(14, 16, 5,5, 1);display.drawRect(14, 28, 5,5, 1);}				  // Side Pads
	    if (y==2) display.drawRect(9, 22, 7,5, 1);									  // Long Pad
	    if (y==3) {display.drawRect(7, 16, 6,5, 1);display.drawRect(7, 28, 6,5, 1);display.drawRect(3, 16, 5,17, 1);} // Reservoir
	    
       	    if (y==4) {display.drawRect(7, 46, 6,5, 1);display.drawRect(7, 46+12, 6,5, 1);display.drawRect(3, 46, 5,17, 1);}
	    if (y==5) display.drawRect(9, 46+6, 7,5, 1);
            if (y==6) {display.drawRect(14, 46, 5,5, 1);display.drawRect(14, 46+12, 5,5, 1);}
	    if (y==7) display.drawRect(x*6+17, 36+16, 5,5, 1);

            } else
           if (x==15)
            {if (y==0) display.drawRect(x*6+17, 6+16, 5,5, 1);
             if (y==1) {display.drawRect(110, 16, 5,5, 1);display.drawRect(110, 28, 5,5, 1);}
	     if (y==2) display.drawRect(113, 22, 7,5, 1);
	     if (y==3) {display.drawRect(116, 16, 6,5, 1);display.drawRect(116, 28, 6,5, 1);display.drawRect(121, 16, 5,17, 1);}

       	     if (y==4) {display.drawRect(116, 46, 6,5, 1);display.drawRect(116, 46+12, 6,5, 1);display.drawRect(121, 46, 5,17, 1);}
	     if (y==5) display.drawRect(113, 46+6, 7,5, 1);
             if (y==6) {display.drawRect(110, 46, 5,5, 1);display.drawRect(110, 46+12, 5,5, 1);}
	     if (y==7) display.drawRect(x*6+17, 36+16, 5,5, 1);

            } else
           display.drawRect(x*6+17, y*6+16, 5,5, 1);
    }


    
if (feedback_flag) {
// draw Feedback
      for (int x = 0; x <64 ; x++) 
   
   if (pad_feedback[x]==1) {
           display.fillRect(pgm_read_byte_near(pad_lookup_x+x)*6+18, pgm_read_byte_near(pad_lookup_y+x)*6+17, 3,3, 1);
    }
    
          for (int x = 0; x <64 ; x++) 
   
   if (pad_feedback[x+64]==1) {
           display.fillRect((15-pgm_read_byte_near(pad_lookup_x+x))*6+18, (7-pgm_read_byte_near(pad_lookup_y+x))*6+17, 3,3, 1);
    }
    }
//draw cursor
if (_show_joy)
 display.drawRect(_joy_x*6+17, _joy_y*6+17, 5,3, 1);
     
// draw Rect


   display.drawLine(22,27,22,51,1);
   display.drawLine(106,27,106,51,1);

display.drawBitmap(2,15,bitmap1,24,19,1);
display.drawBitmap(2,45,bitmap1,24,19,1);

display.drawBitmap(106,15,bitmap2,24,19,1);
display.drawBitmap(106,45,bitmap2,24,19,1);


  display.drawLine(22,15,107,15,1);
  display.drawLine(22,63,107,63,1);

 // display.drawRect(22, 16, 85,48, 1);


// Read SensPad
if (digitalRead(SENSPAD_pin))
{
display.fillRect(8, 1, 115,12, 1);
 display.setTextColor(0); 
 display.setCursor(18,4);
  display.print("INSERT CARTRIDGE");
 display.setTextColor(1);
HV_enable=false;
    digitalWrite(BL_pin, HV_enable); // set BL
} else {HV_enable=true;
    digitalWrite(BL_pin, HV_enable); // set BL
}

// update display
    display.display();

};


void OpenDrop::set_Fluxels(bool fluxels_array[][FluxlPad_heigth])
{

      for (int x = 0; x <(FluxlPad_width) ; x++) 
    for (int y = 0; y <FluxlPad_heigth ; y++) 
Fluxls[x][y]=fluxels_array[x][y];

}


void OpenDrop::drive_Fluxels(void)  // Fill the chip with Fluxls array data

{  
const int chip_delay=5;

    noInterrupts(); // Stop AC while writing.
   digitalWrite(AC_pin, HIGH);
   digitalWrite(POL_pin, !HV_enable);  // clear POL
    delayMicroseconds(chip_delay);

    digitalWrite(LE_pin, !INVERTED);   // clear LE 
    digitalWrite(CLK_pin, !INVERTED);   // clear SCK 
    delayMicroseconds(chip_delay);
 
    for (int i = 0; i <64 ; i++) 

{ digitalWrite(DI_pin,!Fluxls[pgm_read_byte_near(pad_lookup_x+i)][pgm_read_byte_near(pad_lookup_y+i)]);
   delayMicroseconds(chip_delay);

   digitalWrite(CLK_pin, INVERTED);   // set clk
       delayMicroseconds(chip_delay);
   digitalWrite(CLK_pin, !INVERTED);   // clear clk
       delayMicroseconds(chip_delay);
  };
   
    for (int i = 0; i <64 ; i++) 
{
   digitalWrite(DI_pin,!Fluxls[15-pgm_read_byte_near(pad_lookup_x+i)][7-pgm_read_byte_near(pad_lookup_y+i)]);
   delayMicroseconds(chip_delay);

   digitalWrite(CLK_pin, INVERTED);   // set clk
       delayMicroseconds(chip_delay);
   digitalWrite(CLK_pin, !INVERTED);  // clear clk
    delayMicroseconds(chip_delay);
 
   };
   
  
    digitalWrite(LE_pin, INVERTED);   // set LE 
        delayMicroseconds(chip_delay);

    digitalWrite(LE_pin, !INVERTED);   // clear LE 
       delayMicroseconds(chip_delay);

    digitalWrite(BL_pin, HV_enable); // set BL
       delayMicroseconds(chip_delay);

   interrupts();
};

void OpenDrop::read_Fluxels(void) 
{

  bool reading;
  const int chip_delay=5;

    noInterrupts(); // Stop AC while writing.
   digitalWrite(AC_pin, HIGH);
   digitalWrite(POL_pin, INVERTED2);  // clear POL
    delayMicroseconds(chip_delay);

    digitalWrite(LE_pin, !INVERTED);   // clear LE 
    digitalWrite(CLK_pin, !INVERTED);   // clear SCK 
    delayMicroseconds(chip_delay);

// clear driver chip

 	digitalWrite(BL_pin, !INVERTED2); 		//clear BL
 	digitalWrite(DI_pin, !INVERTED);		//clear DI
   	delayMicroseconds(chip_delay);

     for (int i = 0; i <128 ; i++) 
     {   
   digitalWrite(CLK_pin, INVERTED);   // set clk
       delayMicroseconds(chip_delay);
   digitalWrite(CLK_pin, !INVERTED);   // clear clk
       delayMicroseconds(chip_delay);
     }

// turn first Electrode on
 	 digitalWrite(DI_pin, INVERTED);		//set DI

   digitalWrite(CLK_pin, INVERTED);   // set clk
       delayMicroseconds(chip_delay);
   digitalWrite(CLK_pin, !INVERTED);   // clear clk
       delayMicroseconds(chip_delay);

 	 digitalWrite(DI_pin,!INVERTED);		// clear DI
   	 delayMicroseconds(chip_delay);
// read them one by one

   	 digitalWrite(LE_pin, INVERTED);   // set LE 
        delayMicroseconds(chip_delay);

   	 digitalWrite(LE_pin, !INVERTED);   // clear LE 
       delayMicroseconds(chip_delay);


     for (int i = 0; i <128 ; i++)  //128
{

delayMicroseconds(10);

digitalWrite(BL_pin, INVERTED2);	//set BL


reading=digitalRead(FEEDBACK_pin);

pad_feedback[127-i]=reading;

  digitalWrite(BL_pin, !INVERTED2);	//clear BL

  digitalWrite(CLK_pin, !INVERTED);   // clear CLK
    delayMicroseconds(chip_delay);
  digitalWrite(CLK_pin, INVERTED);   // set CLK
    delayMicroseconds(chip_delay);
   
  digitalWrite(LE_pin, !INVERTED);   // clear LE 
    delayMicroseconds(chip_delay);
  digitalWrite(LE_pin, INVERTED);   // set LE 
    delayMicroseconds(chip_delay);
      
   };
  

   digitalWrite(POL_pin, !INVERTED2);  // clear POL
    digitalWrite(BL_pin, !INVERTED); // set BL
       delayMicroseconds(chip_delay);

  interrupts();

}



void OpenDrop::begin(uint16_t freq) {

// Initialize HV Chip

 pinMode(BL_pin, OUTPUT);      //BL pin
  digitalWrite(BL_pin, HV_enable);   // set BL 
  pinStr(BL_pin,1);       //Set Pin output current to 7mA (bug in the Arduino library?)
  
  pinMode(POL_pin, OUTPUT);     //POL pin
  digitalWrite(POL_pin, !HV_enable);   // clr POL 
  pinStr(POL_pin,1);
    
  pinMode(LE_pin, OUTPUT);      //LE pin
  digitalWrite(LE_pin, !INVERTED);   // clr LE 
  pinStr(LE_pin,1);
    
  pinMode(CLK_pin, OUTPUT);     //SCK pin
  digitalWrite(CLK_pin, !INVERTED);   // clr SCK 
  pinStr(CLK_pin,1);
    
  pinMode(DI_pin, OUTPUT);      //DI pin
  digitalWrite(DI_pin, !INVERTED);   // clr DI 
  pinStr(DI_pin,1);

// Initialize  AC

  pinMode(AC_pin, OUTPUT);     //AC pin
  digitalWrite(AC_pin, HIGH);   // set AC 


// Initialize LEDs
  pinMode(LED_D13_pin,OUTPUT); //this configures the LED pin
  digitalWrite(LED_D13_pin,LOW);

  pinMode(LED_Rx_pin,OUTPUT); //this configures the LED pin
  digitalWrite(LED_Rx_pin,LOW);


// Initialize Buttons
  
  pinMode(SW1_pin,INPUT_PULLUP); 
  pinMode(SW2_pin,INPUT_PULLUP); 
  pinMode(SW3_pin,INPUT_PULLUP); 

// Initialize SensePad

  pinMode(SENSPAD_pin,INPUT_PULLUP); 

// Init Voltage Control

  pinMode(VCS_pin, OUTPUT); // set CS pin to output
  pinMode(VSCK_pin, OUTPUT); // set SCK pin to output
  pinMode(VSI_pin, OUTPUT); // set MOSI pin to output
  digitalWrite(VCS_pin, HIGH); // hold slave select 1 pin high, so that chip is not selected to begin with

  this->set_voltage(Voltage_set,AC_flag,AC_frequency);
  tcStartCounter(); //starts the timer


// Init Display
  display.begin(SSD1306_SWITCHCAPVCC);

  // init done
  
  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
 display.setTextSize(1);
 display.setCursor(1,55);
  display.setTextColor(WHITE);
  display.println(version_str);
  display.display();
  delay(500);

  // Clear the buffer.
  display.clearDisplay();
  display.dim(false);


this->set_joy(1,0);
this->show_joy(false);


}

void OpenDrop::update_Drops(void) {

for (int i = 0; i <this->drop_count ; i++) 
{

   if (feedback_flag){

      if ((drops[i].position_x()!=drops[i].next_x())|(drops[i].position_y()!=drops[i].next_y()))
        {Serial.println("run");
         if ((pad_feedback[pgm_read_byte_near(&FluxelID[drops[i].position_x()][drops[i].position_y()])]==0)&(pad_feedback[pgm_read_byte_near(&FluxelID[drops[i].next_x()][drops[i].next_y()])]==1)) {drops[i].begin(drops[i].next_x(),drops[i].next_y());// if at position  
         }; // if not there

      }; //if feedback


} else
drops[i].begin(drops[i].next_x(),drops[i].next_y());

};
}


void OpenDrop::update(void) {

clear_Fluxels();                     //clear Fluxel Array


//Fill Fluxel Array
for (int i = 0; i <this->drop_count ; i++) 
{
  Fluxls[drops[i].next_x()][drops[i].next_y()]=true;
};

if (feedback_flag)
this->read_Fluxels();

this->drive_Fluxels();

this->update_Display();



 if(sound)    {OpenDropAudio.playMe(3);}

}

void OpenDrop::dispense(int reservoir, int delay_us) 
{

Drop *Drop1 = this->getDrop();
Drop *Drop2 = this->getDrop();
Drop *Drop3 = this->getDrop();

//Drop RDrop* this->getDrop();

switch (reservoir)

{
case 1:
Drop1->begin(15,3);
Drop2->begin(15,3);
Drop3->begin(15,3);
this->update(); 
delay(delay_us);
Drop2->begin(15,2);
this->update(); 
delay(delay_us);
Drop1->begin(15,1);
Drop3->begin(15,0);
this->update(); 
delay(delay_us);
Drop1->begin(14,1);
Drop2->begin(14,1);
this->update(); 
delay(delay_us);
Drop2->begin(15,3);
Drop3->begin(15,2);
this->update(); 
delay(delay_us);
Drop2->begin(15,3);
Drop3->begin(15,3);
this->update(); 
delay(delay_us);

break;

case 2:

Drop1->begin(15,4);
Drop2->begin(15,4);
Drop3->begin(15,4);
this->update(); 
delay(delay_us);
Drop2->begin(15,5);
this->update(); 
delay(delay_us);
Drop1->begin(15,6);
Drop3->begin(15,7);
this->update(); 
delay(delay_us);
Drop1->begin(14,6);
Drop2->begin(14,6);
this->update(); 
delay(delay_us);
Drop2->begin(15,4);
Drop3->begin(15,5);
this->update(); 
delay(delay_us);
Drop2->begin(15,4);
Drop3->begin(15,4);
this->update(); 
delay(delay_us);

break;

case 3:

Drop1->begin(0,3);
Drop2->begin(0,3);
Drop3->begin(0,3);
this->update(); 
delay(delay_us);
Drop2->begin(0,2);
this->update(); 
delay(delay_us);
Drop1->begin(0,1);
Drop3->begin(0,0);
this->update(); 
delay(delay_us);
Drop1->begin(1,1);
Drop2->begin(1,1);
this->update(); 
delay(delay_us);
Drop2->begin(0,3);
Drop3->begin(0,2);
this->update(); 
delay(delay_us);
Drop2->begin(0,3);
Drop3->begin(0,3);
this->update(); 
delay(delay_us);


break;

case 4:

Drop1->begin(0,4);
Drop2->begin(0,4);
Drop3->begin(0,4);
this->update(); 
delay(delay_us);
Drop2->begin(0,5);
this->update(); 
delay(delay_us);
Drop1->begin(0,6);
Drop3->begin(0,7);
this->update(); 
delay(delay_us);
Drop1->begin(1,6);
Drop2->begin(1,6);
this->update(); 
delay(delay_us);
Drop2->begin(0,4);
Drop3->begin(0,5);
this->update(); 
delay(delay_us);
Drop2->begin(0,4);
Drop3->begin(0,4);
this->update(); 
delay(delay_us);

break;


}


this->drop_count=this->drop_count-3;
this->update(); 
//delete Drop1;
//delete Drop2;
//delete Drop3;

};


bool OpenDrop::run(void) {
bool transition=false;
this->read_Fluxels();
this->update_Drops();

for (int i = 0; i <this->drop_count ; i++) 
{
bool up =true;
bool down = true;
bool left = true;
bool right = true;
int d1=6000;
int d2=6000;

_runing=true;

_runing= ((drops[i].position_x()!=drops[i].goal_x())|(drops[i].position_y()!=drops[i].goal_y()));
transition=transition|_runing;

if (_runing&!drops[i].is_moving()) {

if (drops[i].position_x()==0) left=false;
if (drops[i].position_x()==(FluxlPad_width-1)) right=false;
if (drops[i].position_y()==0) up=false;
if (drops[i].position_y()==(FluxlPad_heigth-1)) down=false;


if (right) if (!free_Fluxel(drops[i].position_x()+1,drops[i].position_y(),2)) right=false;
if (left) if (!free_Fluxel(drops[i].position_x()-1,drops[i].position_y(),4)) left=false;
if (up) if (!free_Fluxel(drops[i].position_x(),drops[i].position_y()-1,1)) up=false;
if (down) if (!free_Fluxel(drops[i].position_x(),drops[i].position_y()+1,3)) down=false;


if (right) d1=pow((drops[i].position_x()+1)-(drops[i].goal_x()),2)+pow((drops[i].position_y())-(drops[i].goal_y()),2);

if (left) {d2=pow((drops[i].position_x()-1)-(drops[i].goal_x()),2)+pow((drops[i].position_y())-(drops[i].goal_y()),2); if (d2<d1) {right=false;d1=d2;} else left=false;};

/*
if (up) Serial.println("up");
if (down) Serial.println("down");
if (left) Serial.println("left");
if (right) Serial.println("rigth");
*/

if (up) {d2=pow((drops[i].position_x())-(drops[i].goal_x()),2)+pow((drops[i].position_y()-1)-(drops[i].goal_y()),2); if (d2<d1) {left=false;right=false;d1=d2;} else up=false;}

if (down) {d2=pow((drops[i].position_x())-(drops[i].goal_x()),2)+pow((drops[i].position_y()+1)-(drops[i].goal_y()),2); if (d2<d1) {left=false;up=false;right=false;d1=d2;} else down=false;}

/*
Serial.println("pow");
Serial.println(d1);
Serial.println(drops[i].position_x());
Serial.println(drops[i].position_y());
*/




if (up) drops[i].move_up(); else
if (down) drops[i].move_down(); else
if (left) drops[i].move_left(); else
if (right) drops[i].move_right();

};  //if move  

}; // for loop


this->update();

return transition;
};


Drop *OpenDrop::getDrop() {
uint8_t num=this->drop_count;
num++;
this->drop_count=num;
    drops[num-1]._dropnum = num;
//Serial.println(max_drops);
  if (num > max_drops) return NULL;
   
  return &drops[num-1];
}


Drop::Drop(void) {

}

void OpenDrop::set_voltage(uint16_t voltage, bool AC_on,uint16_t frequence)
{
AC_frequency=frequence;
tcConfigure(AC_frequency); //configure the timer to run at <sampleRate>Hertz

 VOLTAGE_set_value=255*((1.5*1500000)/(voltage-1.5)-6800)/10000; // Calculate set value (max 255) for 10kOhm digial potentiometer from reference voltage 1.5V, voltage devider R1.5MOhm / 6.8kOhm, 
 spi_out(VCS_pin, cmd_byte0, VOLTAGE_set_value); // Set Voltage Level / send out data to chip 1, pot 0
Voltage_set=voltage;
AC_flag=AC_on;
}

void OpenDrop::set_joy(uint8_t x,uint8_t y)
{
  _joy_x=x;
  _joy_y=y;
}

void OpenDrop::show_joy(boolean val)
{
 _show_joy=val;
}

void Drop::begin(int x,int y) {
  _pos_x=x;
  _pos_y=y;
  _next_x=x;
  _next_y=y;
  _moving = false;
}

void Drop::move_right(void) {
  
if (_pos_x<FluxlPad_width-2)
  _next_x=_pos_x+1;
  _next_y=_pos_y;

if ((_pos_x==14)&&(_pos_y==1))
 {_next_x=15;_next_y=0;};
if ((_pos_x==15)&&(_pos_y==0))
 {_next_x=15;_next_y=2;};
if ((_pos_x==15)&&(_pos_y==2))
 {_next_x=15;_next_y=3;};
if ((_pos_x==15)&&(_pos_y==1))
 {_next_x=15;_next_y=3;};

if ((_pos_x==14)&&(_pos_y==6))
 {_next_x=15;_next_y=7;};
if ((_pos_x==15)&&(_pos_y==7))
 {_next_x=15;_next_y=5;};
if ((_pos_x==15)&&(_pos_y==5))
 {_next_x=15;_next_y=4;};
if ((_pos_x==15)&&(_pos_y==6))
 {_next_x=15;_next_y=4;};

if ((_pos_x==0)&&(_pos_y==0))
 {_next_x=1;_next_y=1;};
if ((_pos_x==0)&&(_pos_y==2))
 {_next_x=0;_next_y=0;};
if ((_pos_x==0)&&(_pos_y==3))
 {_next_x=0;_next_y=2;};
if ((_pos_x==0)&&(_pos_y==1))
 {_next_x=0;_next_y=0;};

if ((_pos_x==0)&&(_pos_y==7))
 {_next_x=1;_next_y=6;};
if ((_pos_x==0)&&(_pos_y==5))
 {_next_x=0;_next_y=7;};
if ((_pos_x==0)&&(_pos_y==4))
 {_next_x=0;_next_y=5;};
if ((_pos_x==0)&&(_pos_y==6))
 {_next_x=0;_next_y=7;};

  _moving = true;
}

void Drop::move_left(void) {

if (_pos_x>1)
  _next_x=_pos_x-1;
  _next_y=_pos_y;

if ((_pos_x==15)&&(_pos_y==0))
 {_next_x=14;_next_y=1;};
if ((_pos_x==15)&&(_pos_y==2))
 {_next_x=15;_next_y=0;};
if ((_pos_x==15)&&(_pos_y==3))
 {_next_x=15;_next_y=2;};
if ((_pos_x==15)&&(_pos_y==1))
 {_next_x=15;_next_y=0;};

if ((_pos_x==15)&&(_pos_y==7))
 {_next_x=14;_next_y=6;};
if ((_pos_x==15)&&(_pos_y==5))
 {_next_x=15;_next_y=7;};
if ((_pos_x==15)&&(_pos_y==4))
 {_next_x=15;_next_y=5;};
if ((_pos_x==15)&&(_pos_y==6))
 {_next_x=15;_next_y=7;};

if ((_pos_x==1)&&(_pos_y==1))
 {_next_x=0;_next_y=0;};
if ((_pos_x==0)&&(_pos_y==0))
 {_next_x=0;_next_y=2;};
if ((_pos_x==0)&&(_pos_y==2))
 {_next_x=0;_next_y=3;};
if ((_pos_x==0)&&(_pos_y==1))
 {_next_x=0;_next_y=3;};

if ((_pos_x==1)&&(_pos_y==6))
 {_next_x=0;_next_y=7;};
if ((_pos_x==0)&&(_pos_y==7))
 {_next_x=0;_next_y=5;};
if ((_pos_x==0)&&(_pos_y==5))
 {_next_x=0;_next_y=4;};
if ((_pos_x==0)&&(_pos_y==6))
 {_next_x=0;_next_y=4;};


  _moving = true;
}

void Drop::move_up(void) {

if ((_pos_y>0)&&(_pos_x>0)&&(_pos_x<15))
  _next_y=_pos_y-1;
  _next_x=_pos_x;

if ((_pos_x==15)&&(_pos_y==0))
 {_next_x=15;_next_y=1;};
if ((_pos_x==15)&&(_pos_y==2))
 {_next_x=15;_next_y=1;};

if ((_pos_x==15)&&(_pos_y==7))
 {_next_x=15;_next_y=6;};
if ((_pos_x==15)&&(_pos_y==5))
 {_next_x=15;_next_y=6;};

if ((_pos_x==0)&&(_pos_y==0))
 {_next_x=0;_next_y=1;};
if ((_pos_x==0)&&(_pos_y==2))
 {_next_x=0;_next_y=1;};

if ((_pos_x==0)&&(_pos_y==7))
 {_next_x=0;_next_y=6;};
if ((_pos_x==0)&&(_pos_y==5))
 {_next_x=0;_next_y=6;};


  _moving = true;
}

void Drop::move_down(void) {
  
if ((_pos_y<7)&&(_pos_x>0)&&(_pos_x<15))
  _next_y=_pos_y+1;
  _next_x=_pos_x;

if ((_pos_x==15)&&(_pos_y==0))
 {_next_x=15;_next_y=1;};
if ((_pos_x==15)&&(_pos_y==2))
 {_next_x=15;_next_y=1;};

if ((_pos_x==15)&&(_pos_y==7))
 {_next_x=15;_next_y=6;};
if ((_pos_x==15)&&(_pos_y==5))
 {_next_x=15;_next_y=6;};

if ((_pos_x==0)&&(_pos_y==0))
 {_next_x=0;_next_y=1;};
if ((_pos_x==0)&&(_pos_y==2))
 {_next_x=0;_next_y=1;};

if ((_pos_x==0)&&(_pos_y==7))
 {_next_x=0;_next_y=6;};
if ((_pos_x==0)&&(_pos_y==5))
 {_next_x=0;_next_y=6;};

  _moving = true;
}

int Drop::position_x(void) {
  
  return _pos_x;
}


int Drop::position_y(void) {
  
  return _pos_y;
}

int Drop::next_x(void) {
  
  return _next_x;
}


int Drop::next_y(void) {
  
  return _next_y;
}



int Drop::goal_x(void) {
  
  return _goal_x;
}


int Drop::goal_y(void) {
  
  return _goal_y;
}



int Drop::num(void) {
  
  return _dropnum;
}

void Drop::go(int x,int y) {
  _goal_x=x;
  _goal_y=y;
}

bool Drop::is_moving(void) {
  
  return _moving;
}




void Menu(OpenDrop &theOpenDrop)
{
int JOY_value;
int v=Voltage_set;
int f=AC_frequency;
int menu_position=1;
bool AC_state=AC_flag;
bool confirm=false;
bool set_confirm=false;
bool set_sound=sound;
bool nav_release=true;
bool but_release=false;

  display.dim(false);

while (!confirm)
{
display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE,BLACK);
  display.setCursor(15,5);
if (AC_state==true)
  display.print("MODE:     AC"); else
  display.print("MODE:     DC");
  display.setCursor(15,15);
  display.print("VOLTAGE:  ");
  display.print(v);
  display.print(" V");
  display.setCursor(15,25);
  display.print("FREQUENCY:");
 if (f<1000) display.print(" ");
  display.print(f);
  display.print(" Hz");
  display.setCursor(15,35);
if (set_sound==true)
  display.print("SOUND:    ON"); else
  display.print("SOUND:    OFF");


if (!set_confirm){
display.setCursor(15,49);
display.println("SET:      CANCEL");
}
else 
{
display.setCursor(15,49);
display.println("SET:      OK");
}

display.setCursor(0,10*menu_position-5);
if (menu_position==5)display.setCursor(0,49);
  display.print(">");
  display.display();

JOY_value = analogRead(JOY_pin);

if ((JOY_value>725)&&(JOY_value<895)&&(menu_position>1)&&(nav_release)) {menu_position--;nav_release=false;set_confirm=false;}
if ((JOY_value>256)&&(JOY_value<597)&&(menu_position<5)&&(nav_release)) {menu_position++;nav_release=false;}
if (JOY_value>950) nav_release=true;
if (digitalRead(SW1_pin)) but_release=true;
if (!digitalRead(SW1_pin)&&but_release) confirm=true;

switch (menu_position)
{
case 1:
if  (JOY_value<300) AC_state=true;
if  ((JOY_value>600)&&(JOY_value<730)) AC_state=false;
break;

case 2:
if  ((JOY_value>600)&&(JOY_value<730)&&(v>160)) v=v-10;
if  ((JOY_value<300)&&(v<300)) v=v+10;
break;

case 3:
if  ((JOY_value>600)&&(JOY_value<730)&&(f>100)) f=f-50;
if  ((JOY_value<300)&&(f<1500)) f=f+50;

break;

case 4:
if  (JOY_value<300) set_sound=false;
if  ((JOY_value>600)&&(JOY_value<730)) set_sound=true;

break;

case 5:
if  (JOY_value<300) set_confirm=true;
if  ((JOY_value>600)&&(JOY_value<730)) set_confirm=false;
if (!digitalRead(SW3_pin)) confirm=true;
break;


break;
  default:
  break;
}

 }
if (set_confirm) {
    theOpenDrop.set_voltage(v,AC_state,f);
	sound = set_sound;  }
 while(!digitalRead(SW1_pin));
}


