

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

#include <SlowSoftWire.h>

#include <Soft_Adafruit_MCP23008.h>


uint8_t sda_pin=4; 
uint8_t scl_pin=3;


SlowSoftWire softWire = SlowSoftWire(sda_pin, scl_pin,false);


void Adafruit_MCP23008::begin(uint8_t addr) {
  if (addr > 7) {
    addr = 7;
  }
  //i2caddr = addr;
  i2caddr = addr << 1; // Temporary while Spark Core is being fixed

  softWire.begin();

  // set defaults!
  softWire.beginTransmission(MCP23008_ADDRESS | i2caddr);
  softWire.write((byte)MCP23008_IODIR);
  softWire.write((byte)0xFF);  // all inputs
  softWire.write((byte)0x00);
  softWire.write((byte)0x00);
  softWire.write((byte)0x00);
  softWire.write((byte)0x00);
  softWire.write((byte)0x00);
  softWire.write((byte)0x00);
  softWire.write((byte)0x00);
  softWire.write((byte)0x00);
  softWire.write((byte)0x00);	
  softWire.endTransmission();

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
  softWire.beginTransmission(MCP23008_ADDRESS | i2caddr);
  softWire.write((byte)addr);	
  softWire.endTransmission();
  softWire.requestFrom(MCP23008_ADDRESS | i2caddr, 1);
  return softWire.read();
}


void Adafruit_MCP23008::write8(uint8_t addr, uint8_t data) {
  softWire.beginTransmission(MCP23008_ADDRESS | i2caddr);
  softWire.write((byte)addr);
  softWire.write((byte)data);
  softWire.endTransmission();
}


