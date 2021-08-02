// -*- c++ -*-
// Simple sketch to read out BMA020 using SoftI2C

// Readout BMA020 chip


#include <SlowSoftI2CMaster.h>
#include <avr/io.h>


#include <SlowSoftWire.h>

SlowSoftWire Wire = SlowSoftWire(A4, A5);

#define BMAADDR 0x38

int xval, yval, zval;


boolean setControlBits(uint8_t cntr)
{
  Wire.beginTransmission(BMAADDR);
  Wire.write(0x0A);
  Wire.write(cntr);
  return (Wire.endTransmission() == 0);
}

boolean initBma(void)
{
  if (!setControlBits(B00000010)) return false;
  delay(100);
  return true;
}

int readOneVal(void)
{
  uint8_t msb, lsb;
  lsb = Wire.read();
  msb = Wire.read();
  return (int)((msb<<8)|lsb)/64;
}

boolean readBma(void)
{
  xval = 0xFFFF;
  yval = 0xFFFF;
  zval = 0xFFFF;
  Wire.beginTransmission(BMAADDR);
  Wire.write(0x02);
  if (Wire.endTransmission(false) != 0) return false;
  Wire.requestFrom(BMAADDR, 6, true);
  xval = readOneVal();
  yval = readOneVal();
  zval = readOneVal();
  return true;
}



//------------------------------------------------------------------------------
void setup(void) {
  Serial.begin(19200);
  Wire.begin();
  if (!initBma()) {
    Serial.println(F("INIT ERROR"));
    while (1);
  }

}

void loop(void){
  if (!readBma()) {
    Serial.println(F("READ ERROR"));
    while (1);
  }
  Serial.print(F("X="));
  Serial.print(xval);
  Serial.print(F("  Y="));
  Serial.print(yval);
  Serial.print(F("  Z="));
  Serial.println(zval);
  delay(300);
}
