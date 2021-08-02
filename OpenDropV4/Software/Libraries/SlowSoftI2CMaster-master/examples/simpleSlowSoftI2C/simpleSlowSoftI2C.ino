// -*- c++ -*-
// Simple sketch to read out one register of an I2C device
#include <SlowSoftI2CMaster.h>

SlowSoftI2CMaster si = SlowSoftI2CMaster(A4, A5, true);

#define I2C_7BITADDR 0x68 // DS1307
#define MEMLOC 0x0A
#define ADDRLEN 1

  
void setup(void) {
  Serial.begin(38400);
  if (!si.i2c_init())
    Serial.println("I2C init failed");
}

void loop(void){
  if (!si.i2c_start((I2C_7BITADDR<<1)|I2C_WRITE)) {
    Serial.println("I2C device busy");
    return;
  }
  for (byte i=1; i<ADDRLEN; i++) si.i2c_write(0x00);
  si.i2c_write(MEMLOC);
  si.i2c_rep_start((I2C_7BITADDR<<1)|I2C_READ);
  byte val = si.i2c_read(true);
  si.i2c_stop();
  Serial.println(val);
  delay(1000);
}
