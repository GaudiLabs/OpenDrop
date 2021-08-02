// -*- c++ -*-
// Simple sketch to read out one register of an I2C device

#define I2C_7BITADDR 0x68 // DS1307
#define MEMLOC 0x0A
#define ADDRLEN 1

#include <SlowSoftWire.h>

SlowSoftWire Wire = SlowSoftWire(A4, A5);

void setup(void) {
  Serial.begin(57600);
  Wire.begin();
}

void loop(void){
  Wire.beginTransmission(I2C_7BITADDR);
  for (byte i=1; i<ADDRLEN; i++) Wire.write(0x00);
  Wire.write(MEMLOC);
  Wire.endTransmission(false);
  Wire.requestFrom(I2C_7BITADDR,1);
  byte val = Wire.read();
  Serial.println(val);
  delay(1000);
}
