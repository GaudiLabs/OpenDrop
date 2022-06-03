/*--------------------------------------------------------------------
  This file is part of the OpenDrop library
  by Urs Gaudenz, GaudiLabs 
  --------------------------------------------------------------------*/


#if (ARDUINO >= 100)
 #include <Arduino.h>
#else
 #include <WProgram.h>
 #include <pins_arduino.h>
#endif



#define ADAPTER_IODIR 0x00
#define ADAPTER_GPIO 0x01
#define ADAPTER_GETTEMP_H_1 0x02
#define ADAPTER_GETTEMP_L_1 0x03
#define ADAPTER_GETTEMP_H_2 0x04
#define ADAPTER_GETTEMP_L_2 0x05
#define ADAPTER_GETTEMP_H_3 0x06
#define ADAPTER_GETTEMP_L_3 0x07
#define ADAPTER_SETTEMP_H_1 0x08
#define ADAPTER_SETTEMP_L_1 0x09
#define ADAPTER_SETTEMP_H_2 0x0A
#define ADAPTER_SETTEMP_L_2 0x0B
#define ADAPTER_SETTEMP_H_3 0x0C
#define ADAPTER_SETTEMP_L_3 0x0D

#define ADAPTER_ADDR 0x18


void myWireBegin();
uint8_t getAdapterID();
void myPinMode(uint8_t p, uint8_t d);
void myDigitalWrite(uint8_t p, uint8_t d);
uint8_t read8(uint8_t addr);
void write8(uint8_t addr, uint8_t data);

class OpenAdapter
{

 public:

  void begin(void);
  uint8_t get_ID(void);
  void set_Magnet(uint8_t magnet, bool state);
  
  float get_Temp_1(void);
  float get_Temp_2(void);
  float get_Temp_3(void);

  void set_Temp_1(uint8_t temperature);
  void set_Temp_2(uint8_t temperature);
  void set_Temp_3(uint8_t temperature);
  
       
  uint8_t get_Temp_L_1(void);
  uint8_t get_Temp_H_1(void);  
  uint8_t get_Temp_L_2(void);
  uint8_t get_Temp_H_2(void);  
  uint8_t get_Temp_L_3(void);
  uint8_t get_Temp_H_3(void);  
      
 private:
    uint8_t _ID;
   
}; 
  

/* ===================== Adafruit_MCP23008.h ========================= */

#define _BV(bit) (1 << (bit)) // used in LiquidCrystal.cpp

/*************************************************** 
  This is a library for the MCP23008 i2c port expander based on Adafruit Library
  
  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!
  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/

// Don't forget the Wire library
class Adafruit_MCP23008 {
public:
  void begin(uint8_t addr);
  void begin(void);

  void pinModeMCP(uint8_t p, uint8_t d);
  void digitalWrite(uint8_t p, uint8_t d);
  void pullUp(uint8_t p, uint8_t d);
  uint8_t digitalRead(uint8_t p);
  uint8_t readGPIO(void);
  void writeGPIO(uint8_t);

 private:
  uint8_t i2caddr;
  uint8_t read8(uint8_t addr);
  void write8(uint8_t addr, uint8_t data);
};

// Default address is 0x20 - 0x27
#define MCP23008_ADDRESS 0x20

// registers
#define MCP23008_IODIR 0x00
#define MCP23008_IPOL 0x01
#define MCP23008_GPINTEN 0x02
#define MCP23008_DEFVAL 0x03
#define MCP23008_INTCON 0x04
#define MCP23008_IOCON 0x05
#define MCP23008_GPPU 0x06
#define MCP23008_INTF 0x07
#define MCP23008_INTCAP 0x08
#define MCP23008_GPIO 0x09
#define MCP23008_OLAT 0x0A
