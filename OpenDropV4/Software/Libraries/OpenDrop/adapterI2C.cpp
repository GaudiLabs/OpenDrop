/*--------------------------------------------------------------------
  This file is part of the OpenDrop library
  by Urs Gaudenz, GaudiLabs 2022
  --------------------------------------------------------------------*/

#include <Wire.h>
#include "wiring_private.h" // pinPeripheral() function
#include <adapterI2C.h>
#include "hardware_def.h"

  uint8_t gpio=0;
  uint8_t iodir=0;
    
    
TwoWire myWire(&sercom2, 4, 3);

Adafruit_MCP23008 mcp;


void OpenAdapter::begin(void) {

 #if !OpenDropV40_downgrade 
_ID=getAdapterID();
#else
_ID=0;
#endif
 

  
// Init Magnet Shield
if ((_ID&1)==1){


  mcp.begin();      // use default address 0

  mcp.pinModeMCP(STBY_pin, OUTPUT);
  mcp.pinModeMCP(MODE_pin, OUTPUT);
  mcp.pinModeMCP(BOOST_pin, INPUT);
  mcp.pinModeMCP(PHASE_A_pin, OUTPUT);
  mcp.pinModeMCP(ENABLE_A_pin, OUTPUT);
  mcp.pinModeMCP(PHASE_B_pin, OUTPUT);
  mcp.pinModeMCP(ENABLE_B_pin, OUTPUT);

  mcp.digitalWrite(MODE_pin, HIGH);
  mcp.digitalWrite(STBY_pin, HIGH);


  this->set_Magnet(0,LOW);
  this->set_Magnet(1,LOW);
    
 }
 
 
 if ((_ID&2)==2){


  myPinMode(STBY_pin, OUTPUT);
  myPinMode(MODE_pin, OUTPUT);
  myPinMode(BOOST_pin, INPUT);
  myPinMode(PHASE_A_pin, OUTPUT);
  myPinMode(ENABLE_A_pin, OUTPUT);
  myPinMode(PHASE_B_pin, OUTPUT);
  myPinMode(ENABLE_B_pin, OUTPUT);

  myDigitalWrite(MODE_pin, HIGH);
  myDigitalWrite(STBY_pin, HIGH);


  this->set_Magnet(0,LOW);
  this->set_Magnet(1,LOW);

 }
 
 
}


uint8_t OpenAdapter::get_ID(void)
{
return _ID;
 
}


void OpenAdapter::set_Magnet(uint8_t magnet, boolean state)
{

if((_ID&1)==1)
{
if (magnet==1)
    {
    mcp.digitalWrite(PHASE_A_pin, state);
    mcp.digitalWrite(ENABLE_A_pin, HIGH);
    delay (150);
      mcp.digitalWrite(BOOST_pin, LOW);
      mcp.pinModeMCP(BOOST_pin, OUTPUT);
     delay (150);
      mcp.pinModeMCP(BOOST_pin, INPUT);
    mcp.digitalWrite(ENABLE_A_pin, LOW);
      
    }
    else
    {mcp.digitalWrite(PHASE_B_pin, state);
    mcp.digitalWrite(ENABLE_B_pin, HIGH);
    delay (150);
      mcp.digitalWrite(BOOST_pin, LOW);
      mcp.pinModeMCP(BOOST_pin, OUTPUT);
     delay (150);
      mcp.pinModeMCP(BOOST_pin, INPUT);
    mcp.digitalWrite(ENABLE_B_pin, LOW);
    }
  }

if((_ID&2)==2)
{


if (magnet==1)
    {
    myDigitalWrite(PHASE_A_pin, state);
    myDigitalWrite(ENABLE_A_pin, HIGH);
    delay (150);
      myDigitalWrite(BOOST_pin, LOW);
      myPinMode(BOOST_pin, OUTPUT);
     delay (150);
      myPinMode(BOOST_pin, INPUT);
    myDigitalWrite(ENABLE_A_pin, LOW);
      
    }
    else
    {myDigitalWrite(PHASE_B_pin, state);
    myDigitalWrite(ENABLE_B_pin, HIGH);
    delay (150);
      myDigitalWrite(BOOST_pin, LOW);
      myPinMode(BOOST_pin, OUTPUT);
     delay (150);
      myPinMode(BOOST_pin, INPUT);
    myDigitalWrite(ENABLE_B_pin, LOW);
    }
  }
    
}
  

float OpenAdapter::get_Temp_1(void)
{float temp=0;
if ((_ID&2)==2) temp=float(read8(ADAPTER_GETTEMP_L_1))/100+read8(ADAPTER_GETTEMP_H_1);
return temp;
};

float OpenAdapter::get_Temp_2(void)
{float temp=0;
if ((_ID&2)==2) temp=float(read8(ADAPTER_GETTEMP_L_2))/100+read8(ADAPTER_GETTEMP_H_2);
return temp;
};

float OpenAdapter::get_Temp_3(void)
{float temp=0;
if ((_ID&2)==2) temp=float(read8(ADAPTER_GETTEMP_L_3))/100+read8(ADAPTER_GETTEMP_H_3);
return temp;
};



void OpenAdapter::set_Temp_1(uint8_t temperature)
{
if ((_ID&2)==2) write8(ADAPTER_SETTEMP_H_1,temperature);
};

void OpenAdapter::set_Temp_2(uint8_t temperature)
{
if ((_ID&2)==2)write8(ADAPTER_SETTEMP_H_2,temperature);
};

void OpenAdapter::set_Temp_3(uint8_t temperature)
{
if ((_ID&2)==2)write8(ADAPTER_SETTEMP_H_3,temperature);
};
  
  
  
     
uint8_t OpenAdapter::get_Temp_L_1(void) {return read8(ADAPTER_GETTEMP_L_1);};
uint8_t OpenAdapter::get_Temp_H_1(void) {return read8(ADAPTER_GETTEMP_H_1);}; 
uint8_t OpenAdapter::get_Temp_L_2(void) {return read8(ADAPTER_GETTEMP_L_2);};
uint8_t OpenAdapter::get_Temp_H_2(void) {return read8(ADAPTER_GETTEMP_H_2);};  
uint8_t OpenAdapter::get_Temp_L_3(void) {return read8(ADAPTER_GETTEMP_L_3);};
uint8_t OpenAdapter::get_Temp_H_3(void) {return read8(ADAPTER_GETTEMP_H_3);}; 
  
  
  
void myWireBegin()
{
 myWire.begin();
  // Assign pins 4 & 3 to SERCOM functionality
  pinPeripheral(4, PIO_SERCOM_ALT);
  pinPeripheral(3, PIO_SERCOM_ALT);
  }
  
  
  

  
  
  
/* ===================== Adafruit_MCP23008.cpp ========================= */

/*************************************************** 
  This is a library for the MCP23008 i2c port expander
  These displays use I2C to communicate, 2 pins are required to  
  interface
  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!
  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/


#define I2C_7BITADDR 0x68 // DS1307
#define MEMLOC 0x0A
#define ADDRLEN 1



void Adafruit_MCP23008::begin(uint8_t addr) {
  if (addr > 7) {
    addr = 7;
  }
  //i2caddr = addr;
  i2caddr = addr << 1; // Temporary while Spark Core is being fixed


  // set defaults!
  myWire.beginTransmission(MCP23008_ADDRESS | i2caddr);
  myWire.write((uint8_t)MCP23008_IODIR);
  myWire.write((uint8_t)0xFF);  // all inputs
  myWire.write((uint8_t)0x00);
  myWire.write((uint8_t)0x00);
  myWire.write((uint8_t)0x00);
  myWire.write((uint8_t)0x00);
  myWire.write((uint8_t)0x00);
  myWire.write((uint8_t)0x00);
  myWire.write((uint8_t)0x00);
  myWire.write((uint8_t)0x00);
  myWire.write((uint8_t)0x00);	
  myWire.endTransmission();

}

void Adafruit_MCP23008::begin(void) {
  begin(0);
}

void Adafruit_MCP23008::pinModeMCP(uint8_t p, uint8_t d) {
  uint8_t iodir;
  

  // only 8 bits!
  if (p > 7)
    return;
  
  iodir = read8(MCP23008_IODIR);

  // set the pin and direction
  if (d == INPUT) {
    iodir |= 1 << p; 
  } else {
    iodir &= ~(1 << p);
  }

  // write the new IODIR
  write8(MCP23008_IODIR, iodir);
}

uint8_t Adafruit_MCP23008::readGPIO(void) {
  // read the current GPIO input 
  return read8(MCP23008_GPIO);
}

void Adafruit_MCP23008::writeGPIO(uint8_t gpio) {
  write8(MCP23008_GPIO, gpio);
}


void Adafruit_MCP23008::digitalWrite(uint8_t p, uint8_t d) {
  uint8_t gpio;
  
  // only 8 bits!
  if (p > 7)
    return;

  // read the current GPIO output latches
  gpio = readGPIO();

  // set the pin and direction
  if (d == HIGH) {
    gpio |= 1 << p; 
  } else {
    gpio &= ~(1 << p);
  }

  // write the new GPIO
  writeGPIO(gpio);
}

void Adafruit_MCP23008::pullUp(uint8_t p, uint8_t d) {
  uint8_t gppu;
  
  // only 8 bits!
  if (p > 7)
    return;

  gppu = read8(MCP23008_GPPU);
  // set the pin and direction
  if (d == HIGH) {
    gppu |= 1 << p; 
  } else {
    gppu &= ~(1 << p);
  }
  // write the new GPIO
  write8(MCP23008_GPPU, gppu);
}

uint8_t Adafruit_MCP23008::digitalRead(uint8_t p) {
  // only 8 bits!
  if (p > 7)
    return 0;

  // read the current GPIO
  return (readGPIO() >> p) & 0x1;
}

uint8_t Adafruit_MCP23008::read8(uint8_t addr) {
  myWire.beginTransmission(MCP23008_ADDRESS | i2caddr);
  myWire.write((uint8_t)addr);	
  myWire.endTransmission();
  myWire.requestFrom(MCP23008_ADDRESS | i2caddr, 1);
  return myWire.read();
}


void Adafruit_MCP23008::write8(uint8_t addr, uint8_t data) {
  myWire.beginTransmission(MCP23008_ADDRESS | i2caddr);
  myWire.write((uint8_t)addr);
  myWire.write((uint8_t)data);
  myWire.endTransmission();
}

/* ===================== I2C Adapter Interface ========================= */

void myDigitalWrite(uint8_t p, uint8_t d) {

  
  // only 8 bits!
  if (p > 7)
    return;

  // read the current GPIO output latches
  

  // set the pin and direction
  if (d == HIGH) {
    gpio |= 1 << p; 
  } else {
    gpio &= ~(1 << p);
  }

  // write the new GPIO
  write8(ADAPTER_GPIO,gpio);
}

void myPinMode(uint8_t p, uint8_t d) {

  

  // only 8 bits!
  if (p > 7)
    return;
  

  // set the pin and direction
  if (d == INPUT) {
    iodir |= 1 << p; 
  } else {
    iodir &= ~(1 << p);
  }

  // write the new IODIR
  write8(ADAPTER_IODIR, iodir);
  
}



uint8_t read8(uint8_t addr) {
  myWire.beginTransmission(ADAPTER_ADDR);
  myWire.write((uint8_t)addr);  
  myWire.endTransmission();
  myWire.requestFrom(ADAPTER_ADDR, 1);
  return myWire.read();
}

void write8(uint8_t addr, uint8_t data) {
  myWire.beginTransmission(ADAPTER_ADDR);
  myWire.write((uint8_t)addr);
  myWire.write((uint8_t)data);
  myWire.endTransmission();
}



/* ===================== Adapter Check ========================= */
//Check if Adapter and what type

uint8_t getAdapterID()
{
  #define MCP23008_ADDRESS 0x20
  uint8_t adapterID = 0;
  uint8_t error;

// Test if PullUp is present
  pinMode(P1_pin, INPUT_PULLDOWN); 
  
  /* Configure the input parameters.

   - GAIN_DIV2 means that the input voltage is halved. This is important
     because the voltage reference is 1/2 of VCCA. So if you want to
     measure 0-3.3v, you need to halve the input as well.

   - MUXNEG_GND means that the ADC should compare the input value to GND.

   - MUXPOST_PIN3 means that the ADC should read from AIN3, or PB09.
     This is A2 on the Feather M0 board.
*/
ADC->INPUTCTRL.reg = ADC_INPUTCTRL_GAIN_DIV2 |
                     ADC_INPUTCTRL_MUXNEG_GND |
                     ADC_INPUTCTRL_MUXPOS_PIN16;


/* Set PB09 as an input pin. */
PORT->Group[1].DIRCLR.reg = PORT_PA08;

/* Enable the peripheral multiplexer for PB09. */
PORT->Group[1].PINCFG[8].reg |= PORT_PINCFG_PMUXEN;

/* Set PB09 to function B which is analog input. */
PORT->Group[1].PMUX[13].reg = PORT_PMUX_PMUXO_B;

/* Wait for bus synchronization. */
while (ADC->STATUS.bit.SYNCBUSY) {};

/* Enable the ADC. */
ADC->CTRLA.bit.ENABLE = true;

/* Wait for bus synchronization. */
while (ADC->STATUS.bit.SYNCBUSY) {};

/* Start the ADC using a software trigger. */
ADC->SWTRIG.bit.START = true;

/* Wait for the result ready flag to be set. */
while (ADC->INTFLAG.bit.RESRDY == 0);

/* Clear the flag. */
ADC->INTFLAG.reg = ADC_INTFLAG_RESRDY;

/* Read the value. */

// If PullUp is present check for I2C Addresses

if ((ADC->RESULT.reg)>50) {

//TwoWire myWire(&sercom2, 4, 3);
  myWire.begin();
  // Assign pins 4 & 3 to SERCOM functionality
  pinPeripheral(P1_pin, PIO_SERCOM_ALT);
  pinPeripheral(P2_pin, PIO_SERCOM_ALT);

  
  myWire.beginTransmission(MCP23008_ADDRESS);  
  error = myWire.endTransmission();
  if (error==0) adapterID+=1;

  myWire.beginTransmission(ADAPTER_ADDR);  
  error = myWire.endTransmission();
  if (error==0) adapterID+=2;

}
     
 
return adapterID;

  }


