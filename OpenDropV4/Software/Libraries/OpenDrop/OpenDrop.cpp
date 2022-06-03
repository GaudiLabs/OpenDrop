/*--------------------------------------------------------------------
  This file is part of the OpenDrop library
  by Urs Gaudenz, GaudiLabs 2022
  --------------------------------------------------------------------*/


#include "OpenDrop.h"
#include "hardware_def.h"
#include "bitmap.h"
#include "Adafruit_GFX.h"
#include <Adafruit_SSD1306.h>
#include <OpenDropAudio.h>
#include <SAMD_AnalogCorrection.h>
#include "adapterI2C.h"
#include "FlashStorage.h"

char version_str[] = "V42.10." ;

uint8_t OpenDropID=0;     // defined in code

bool Fluxls[FluxlPad_width][FluxlPad_heigth];
bool Fluxls_feedback[FluxlPad_width][FluxlPad_heigth];
byte pad_feedback [128];


#define SETTING_Size 6
typedef struct {
int value[SETTING_Size];
} EEprom;

FlashStorage(my_flash_store, EEprom);
EEprom settings;



uint32_t AC_frequency=1000; // Frequency in Hz
uint16_t Voltage_set=240; // Voltage in Volt
uint16_t Volateg_tollerance=25;

boolean AC_flag=false;     // AC or DC voltage selection
boolean HV_enable=false;     // enable high voltage 
boolean HV_set_ok=false;     // is set voltage reached

bool state = 0; //just for an example

float VOLTAGE_value;
long VOLTAGE_set_value;

bool sound=sound_flag;
bool feedback=feedback_flag;



// used for SPI bitbang
byte cmd_byte0 = B00010001; // command byte to write to pot 0, from the MCP42XXX datasheet
byte work = B00000000; // setup a working byte, used to bit shift the data out


Adafruit_SSD1306 display(OLED_MOSI,OLED_CLK,OLED_DC, OLED_RESET, OLED_CS);


OpenAdapter Adapter;


//********** Interupt Functions **********


//this function gets called by the interrupt at <sampleRate>Hertz
void TC4_Handler (void) {
 if ((AC_flag==true)&&(HV_enable==true)){
  if(state == true) {
      digitalWrite(AC_pin, LOW);
      delayMicroseconds(68); //67
      digitalWrite(POL_pin, opto_LOW);
   // digitalWrite(LED_D13_pin,LOW);
  } else {
      digitalWrite(AC_pin, HIGH);
      delayMicroseconds(74); 
      digitalWrite(POL_pin, opto_HIGH);
   // digitalWrite(LED_D13_pin,HIGH);
  }

 } else 

 {
   digitalWrite(AC_pin, HIGH);
   digitalWrite(POL_pin, opto_HIGH);
  
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


// setting AnalogRead parameters , code by adamgarbo 
void configureAdc()
{
  ADC->CTRLA.bit.ENABLE = 0;                      // Disable ADC
  ADC->CTRLB.reg = ADC_CTRLB_PRESCALER_DIV512 |   // Divide Clock ADC GCLK by 512 (48MHz/512 = 93.7kHz)
                   ADC_CTRLB_RESSEL_10BIT;        // Set ADC resolution to 12 bits
  
  while (ADC->STATUS.bit.SYNCBUSY);               // Wait for synchronization
  ADC->SAMPCTRL.reg = ADC_SAMPCTRL_SAMPLEN(63);   // Set Sampling Time Length (341.33 us)
  ADC->AVGCTRL.reg = ADC_AVGCTRL_SAMPLENUM_1 |  // Configure multisampling
                     ADC_AVGCTRL_ADJRES(0);       // Configure averaging
  while (ADC->STATUS.bit.SYNCBUSY);               // Wait for synchronization
  ADC->CTRLA.bit.ENABLE = 1;                      // Enable ADC
  while (ADC->STATUS.bit.SYNCBUSY);               // Wait for synchronization

  // Apply ADC gain and offset error calibration correction
//  analogReadCorrection(23, 2060); //CORREN = 1
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

 
  if (feedback) {  
 this->read_Fluxels();
  for (int x = 0; x <(FluxlPad_width) ; x++) 
    for (int y = 0; y <FluxlPad_heigth ; y++) 
   if (Fluxls_feedback[x][y]){
       if (x==0)
	{if (y==0) display.drawRect(x*6+18, 6+17, 3,3, 1);							          // Entrance
            if (y==1) {display.drawRect(15, 17, 3,3, 1);display.drawRect(15, 29, 3,3, 1);}				  // Side Pads
	    if (y==2) display.drawRect(10, 23, 5,3, 1);									  // Long Pad
	    if (y==3) {display.drawRect(8, 17, 4,3, 1);display.drawRect(8, 29, 4,3, 1);display.drawRect(4, 17, 3,15, 1);} // Reservoir
	    
      	     if (y==4) {display.drawRect(8, 47, 4,3, 1);display.drawRect(8, 46+13, 4,3, 1);display.drawRect(4, 47, 3,15, 1);}
	    if (y==5) display.drawRect(10, 46+7, 5,3, 1);
            if (y==6) {display.drawRect(15, 47, 3,3, 1);display.drawRect(15, 46+13, 3,3, 1);}
	    if (y==7) display.drawRect(x*6+17, 36+16, 5,5, 1);

            } else
           if (x==15)
            {if (y==0) display.drawRect(x*6+18, 6+17, 3,3, 1);
             if (y==1) {display.drawRect(111, 17, 3,3, 1);display.drawRect(111, 29, 3,3, 1);}
	     if (y==2) display.drawRect(114, 23, 5,3, 1);
	     if (y==3) {display.drawRect(117, 17, 4,3, 1);display.drawRect(117, 29, 4,3, 1);display.drawRect(122, 17, 3,15, 1);}

       	     if (y==4) {display.drawRect(117, 47, 4,3, 1);display.drawRect(117, 46+13, 4,3, 1);display.drawRect(122, 47, 3,15, 1);}
	     if (y==5) display.drawRect(114, 46+7, 5,3, 1);
             if (y==6) {display.drawRect(111, 47, 3,3, 1);display.drawRect(111, 46+13, 3,3, 1);}
	     if (y==7) display.drawRect(x*6+18, 36+17, 3,3, 1);

            } else
           display.drawRect(x*6+18, y*6+17, 3,3, 1);
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
display.fillRect(4, 33, 121,12, 1);
 display.setTextColor(0); 
 display.setCursor(18,36);
  display.print("INSERT CARTRIDGE");
 display.setTextColor(1);
HV_enable=false;
    digitalWrite(BL_pin, HV_enable); // set BL
} else {HV_enable=true;
    digitalWrite(BL_pin, HV_enable); // set BL
}


// Volatge Check
if (abs(VOLTAGE_value-Voltage_set)<Volateg_tollerance) HV_set_ok=true;
if (analogRead(V_USB_pin)<800) HV_set_ok=false;

if ((HV_set_ok)&&((abs(VOLTAGE_value-Voltage_set)>Volateg_tollerance)) ) 
{display.fillRect(4, 33, 121,12, 1);
 display.setTextColor(0); 
 display.setCursor(18,36);
  display.print("UNDER VOLTAGE!");
   display.setTextColor(1); 
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

bool OpenDrop::get_Fluxel(int x, int y)
{
return Fluxls_feedback[x][y];
}




void OpenDrop::drive_Fluxels(void)  // Fill the chip with Fluxls array data

{  
const int chip_delay=5;

    noInterrupts(); // Stop AC while writing.
   digitalWrite(AC_pin, HIGH);
   digitalWrite(POL_pin, opto_HIGH);  // clear POL
    delayMicroseconds(chip_delay);

    digitalWrite(LE_pin, opto_LOW);   
    digitalWrite(CLK_pin, opto_LOW);    
    delayMicroseconds(chip_delay);
 
    for (int i = 0; i <64 ; i++) 

{ digitalWrite(DI_pin,Fluxls[pgm_read_byte_near(pad_lookup_x+i)][pgm_read_byte_near(pad_lookup_y+i)]);
   delayMicroseconds(chip_delay);

   digitalWrite(CLK_pin, opto_HIGH);  
       delayMicroseconds(chip_delay);
   digitalWrite(CLK_pin, opto_LOW);   
       delayMicroseconds(chip_delay);
  };
   
    for (int i = 0; i <64 ; i++) 
{
   digitalWrite(DI_pin,Fluxls[15-pgm_read_byte_near(pad_lookup_x+i)][7-pgm_read_byte_near(pad_lookup_y+i)]);
   delayMicroseconds(chip_delay);

   digitalWrite(CLK_pin, opto_HIGH);   
       delayMicroseconds(chip_delay);
   digitalWrite(CLK_pin, opto_LOW);  
    delayMicroseconds(chip_delay);
 
   };
   
  
    digitalWrite(LE_pin, opto_HIGH);    
        delayMicroseconds(chip_delay);

    digitalWrite(LE_pin, opto_LOW);    
       delayMicroseconds(chip_delay);

    digitalWrite(BL_pin, HV_enable); // set BL
       delayMicroseconds(chip_delay);

   interrupts();
};

void OpenDrop::read_Fluxels(void) 
{

  bool reading;
  const int chip_delay=5;
  
  if (HV_enable){
  
   pinMode(FEEDBACK_pin, INPUT);

    noInterrupts(); // Stop AC while writing.
   digitalWrite(AC_pin, HIGH);
   
   // clear driver chip
   digitalWrite(POL_pin, opto_HIGH);  
   digitalWrite(BL_pin, opto_LOW); 		//clear BL
    	
    digitalWrite(LE_pin, opto_LOW);   // clear LE 
    digitalWrite(CLK_pin, opto_LOW);   // clear SCK 
    digitalWrite(DI_pin, opto_LOW);		//clear DI
     	
    delayMicroseconds(chip_delay);


     for (int i = 0; i <128 ; i++) 
     {   
   digitalWrite(CLK_pin, opto_HIGH);   // set clk
       delayMicroseconds(chip_delay);
   digitalWrite(CLK_pin, opto_LOW);   // clear clk
       delayMicroseconds(chip_delay);
     }

// turn first Electrode on
 	 digitalWrite(DI_pin, opto_HIGH);		//set DI

   digitalWrite(CLK_pin, opto_HIGH);   // set clk
       delayMicroseconds(chip_delay);
   digitalWrite(CLK_pin, opto_LOW);   // clear clk
       delayMicroseconds(chip_delay);

 	 digitalWrite(DI_pin,opto_LOW);		// clear DI
   	 delayMicroseconds(chip_delay);


   	 digitalWrite(LE_pin, opto_HIGH);   // set LE 
        delayMicroseconds(chip_delay);

   	 digitalWrite(LE_pin, opto_LOW);   // clear LE 
       delayMicroseconds(chip_delay);

// read them one by one


     for (int i = 0; i <128 ; i++)  //128
{

digitalWrite(BL_pin, opto_HIGH);	//set BL
delayMicroseconds(7);

reading=!digitalRead(FEEDBACK_pin);

pad_feedback[127-i]=reading;

if (i<64)
Fluxls_feedback[15-pgm_read_byte_near(pad_lookup_x+63-i)][7-pgm_read_byte_near(pad_lookup_y+63-i)]=reading;
else
Fluxls_feedback[pgm_read_byte_near(pad_lookup_x+127-i)][pgm_read_byte_near(pad_lookup_y+127-i)]=reading;


  digitalWrite(BL_pin, opto_LOW);	//clear BL

  digitalWrite(CLK_pin, opto_LOW);   // clear CLK
   digitalWrite(LE_pin, opto_LOW);   // clear LE 
    delayMicroseconds(chip_delay);
  digitalWrite(CLK_pin, opto_HIGH);   // set CLK
    delayMicroseconds(chip_delay);
  digitalWrite(LE_pin, opto_HIGH);   // set LE 
    delayMicroseconds(chip_delay);
      
   };
  
  this->drive_Fluxels();
   
  digitalWrite(POL_pin, opto_HIGH);   // clr POL 
  digitalWrite(BL_pin, HV_enable); // set BL

    interrupts();
    } // HVEnable
}



void OpenDrop::begin(char code_str[]) {




  
// Initialize HV Chip

   //     pinStr(P1_pin,1);       //Set Pin output current to 7mA (bug in the Arduino library?)

   //     pinStr(P2_pin,1);       //Set Pin output current to 7mA (bug in the Arduino library?)

  pinMode(BL_pin, OUTPUT);      //BL pin
  digitalWrite(BL_pin, HV_enable);   // set BL 
  pinStr(BL_pin,1);       //Set Pin output current to 7mA (bug in the Arduino library?)
  
  pinMode(POL_pin, OUTPUT);     //POL pin
  digitalWrite(POL_pin, opto_HIGH);   // clr POL 
  pinStr(POL_pin,1);
    
  pinMode(LE_pin, OUTPUT);      //LE pin
  digitalWrite(LE_pin, opto_LOW);   // clr LE 
  pinStr(LE_pin,1);
    
  pinMode(CLK_pin, OUTPUT);     //SCK pin
  digitalWrite(CLK_pin, opto_LOW);   // clr SCK 
  pinStr(CLK_pin,1);
    
  pinMode(DI_pin, OUTPUT);      //DI pin
  digitalWrite(DI_pin, opto_LOW);   // clr DI 
  pinStr(DI_pin,1);


// Initialize  AC

  pinMode(AC_pin, OUTPUT);     //AC pin
  digitalWrite(AC_pin, HIGH);   // set AC 

// Initialize ADC

  configureAdc();


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


    settings=my_flash_store.read();

     if (settings.value[5]==0) {
	settings.value[0]=AC_flag;
	settings.value[1]=Voltage_set;
	settings.value[2]=AC_frequency;
	settings.value[3]=sound;
	settings.value[4]=feedback;
	settings.value[5]=1;
	my_flash_store.write(settings);
	
     };

    sound=settings.value[3];
    feedback=settings.value[4];
    
  this->set_voltage(settings.value[1],settings.value[0],settings.value[2]);
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
  display.print(version_str);
  display.println(code_str);

  display.display();
 
  
// Init Adapters
  Adapter.begin();
  
   pinMode(AC_pin, OUTPUT);     //Re-Init AC pin (gets messed up in getAdapterID)
  digitalWrite(AC_pin, HIGH);   //Re-Init set AC  (gets messed up in getAdapterID)
  
  OpenDropID=(board_ID<<4)|Adapter.get_ID(); // OpenDrop ID = board_ID higher 4 bits, Adapter ID lower 4 bits 
   display.setCursor(95,55);
   display.print("ID:");
   display.println(OpenDropID,HEX);
   display.display(); 
 
  
  delay(400);

  // Clear the buffer.
  display.clearDisplay();
  display.dim(false);


this->set_joy(1,0);
this->show_joy(false);


}

void OpenDrop::update_Drops(void) {

for (int i = 0; i <this->drop_count ; i++) 
{

   if (closedloop_flag){

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

#if OpenDropV40_downgrade
 VOLTAGE_set_value=255*((1.5*1500000)/(voltage-1.5)-6800)/10000; // Calculate set value (max 255) for 10kOhm digial potentiometer from reference voltage 1.5V, voltage devider R1.5MOhm / 6.8kOhm, 
#else
 VOLTAGE_set_value=255*((1.5*1500000)/(voltage-1.5)-6800)/50000; // Calculate set value (max 255) for 50kOhm digial potentiometer from reference voltage 1.5V, voltage divider R1.5MOhm / 6.8kOhm, 
 #endif
 
 
 spi_out(VCS_pin, cmd_byte0, VOLTAGE_set_value); // Set Voltage Level / send out data to chip 1, pot 0
Voltage_set=voltage;
AC_flag=AC_on;
}


void OpenDrop::set_Magnet(uint8_t magnet, bool state)
{ 
Adapter.set_Magnet(magnet,state);
}

uint8_t OpenDrop::get_ID()
{
return OpenDropID;
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

void OpenDrop::show_feedback(boolean val)
{
 feedback=val;
}

float OpenDrop::get_Temp_1(void){return Adapter.get_Temp_1();};
float OpenDrop::get_Temp_2(void){return Adapter.get_Temp_2();};
float OpenDrop::get_Temp_3(void){return Adapter.get_Temp_3();};

void OpenDrop::set_Temp_1(uint8_t temperature){Adapter.set_Temp_1(temperature);};
void OpenDrop::set_Temp_2(uint8_t temperature){Adapter.set_Temp_2(temperature);};
void OpenDrop::set_Temp_3(uint8_t temperature){Adapter.set_Temp_3(temperature);};
  
uint8_t OpenDrop::get_Temp_L_1(void){return Adapter.get_Temp_L_1();};
uint8_t OpenDrop::get_Temp_H_1(void){return Adapter.get_Temp_H_1();}; 
uint8_t OpenDrop::get_Temp_L_2(void){return Adapter.get_Temp_L_2();};
uint8_t OpenDrop::get_Temp_H_2(void){return Adapter.get_Temp_H_2();}; 
uint8_t OpenDrop::get_Temp_L_3(void){return Adapter.get_Temp_L_3();};
uint8_t OpenDrop::get_Temp_H_3(void){return Adapter.get_Temp_H_3();}; 
  
  

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
bool set_feedback=feedback;
bool set_sound=sound;
bool nav_release=true;
bool but_release=false;

  display.dim(false);

while (!confirm)
{
display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE,BLACK);
  display.setCursor(15,2);
if (AC_state==true)
  display.print("MODE:     AC"); else
  display.print("MODE:     DC");
  display.setCursor(15,12);
  display.print("VOLTAGE:  ");
  display.print(v);
  display.print(" V");
  display.setCursor(15,22);
  display.print("FREQUENCY:");
 if (f<1000) display.print(" ");
  display.print(f);
  display.print(" Hz");
  display.setCursor(15,32);
if (set_sound==true)
  display.print("SOUND:    ON"); else
  display.print("SOUND:    OFF");
  display.setCursor(15,42);
if (set_feedback==true)
  display.print("FEEDBACK: ON"); else
  display.print("FEEDBACK: OFF");

if (!set_confirm){
display.setCursor(15,55);
display.println("SET:      CANCEL");
}
else 
{
display.setCursor(15,55);
display.println("SET:      OK");
}

display.setCursor(0,10*menu_position-8);
if (menu_position==6)display.setCursor(0,55);
  display.print(">");
  display.display();

JOY_value = analogRead(JOY_pin);

if ((JOY_value>725)&&(JOY_value<895)&&(menu_position>1)&&(nav_release)) {menu_position--;nav_release=false;set_confirm=false;}
if ((JOY_value>256)&&(JOY_value<597)&&(menu_position<6)&&(nav_release)) {menu_position++;nav_release=false;}
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
if  ((JOY_value>600)&&(JOY_value<730)&&(v>50)) v=v-10;
if  ((JOY_value<300)&&(v<280)) v=v+10;
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
if  (JOY_value<300) set_feedback=true;
if  ((JOY_value>600)&&(JOY_value<730)) set_feedback=false;

break;

case 6:
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
	sound = set_sound; 
	feedback=set_feedback;
	settings.value[0]=AC_state;
	settings.value[1]=v;
	settings.value[2]=f;
	settings.value[3]=set_sound;
	settings.value[4]=set_feedback;
	my_flash_store.write(settings);
	
	HV_set_ok=false;
	 }
 while(!digitalRead(SW1_pin));
}


