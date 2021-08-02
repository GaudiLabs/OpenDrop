// -*- c++ -*-
// Simple sketch to read out BMA020 using SlowSoftI2CMaster

// Readout BMA020 chip


#include <SlowSoftI2CMaster.h>
#include <avr/io.h>


SlowSoftI2CMaster si = SlowSoftI2CMaster(A4, A5);

#define BMAADDR 0x70
#define LEDPIN 13

int xval, yval, zval;


boolean setControlBits(uint8_t cntr)
{
  if (!si.i2c_start(BMAADDR | I2C_WRITE)) {
    return false;
  }
  if (!si.i2c_write(0x0A)) {
    return false;
  }
  if (!si.i2c_write(cntr)) {
    return false;
  }
  si.i2c_stop();
  Serial.println(F("stop done"));
  return true;
}

boolean initBma(void)
{
  if (!setControlBits(B00000010)) return false;
  delay(100);
  return true;
}

int readOneVal(boolean last)
{
  uint8_t msb, lsb;
  lsb = si.i2c_read(false);
  msb = si.i2c_read(last);
  if (last) si.i2c_stop();
  return (int)((msb<<8)|lsb)/64;
}

boolean readBma(void)
{
  xval = 0xFFFF;
  yval = 0xFFFF;
  zval = 0xFFFF;
  if (!si.i2c_start(BMAADDR | I2C_WRITE)) return false;
  if (!si.i2c_write(0x02)) return false;
  if (!si.i2c_rep_start(BMAADDR | I2C_READ)) return false;
  xval = readOneVal(false);
  yval = readOneVal(false);
  zval = readOneVal(true);
  return true;
}



//------------------------------------------------------------------------------
void setup(void) {
  Serial.begin(19200);
  si.i2c_init();
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
