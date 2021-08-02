// -*-c++-*-
// Scan I2C bus for device responses

#include <SlowSoftI2CMaster.h>
#include <avr/io.h>

// use true as third arg, if you do not have external pullups
SlowSoftI2CMaster si = SlowSoftI2CMaster(A4, A5, true);

//------------------------------------------------------------------------------

void setup(void) {
  Serial.begin(19200); 
  Serial.println(F("Intializing ..."));
  if (!si.i2c_init()) 
    Serial.println(F("Initialization error. SDA or SCL are low"));
  else
    Serial.println(F("...done"));
}

void loop(void)
{
  uint8_t add = 0;
  int found = false;
  Serial.println("Scanning ...");

  Serial.println("       8-bit 7-bit addr");
  // try read
  do {
    if (si.i2c_start(add | I2C_READ)) {
      found = true;
      si.i2c_read(true);
      si.i2c_stop();
      Serial.print("Read:   0x");
      if (add < 0x0F) Serial.print(0, HEX);
      Serial.print(add+I2C_READ, HEX);
      Serial.print("  0x");
      if (add>>1 < 0x0F) Serial.print(0, HEX);
      Serial.println(add>>1, HEX);
    } else si.i2c_stop();
    add += 2;
  } while (add);

  // try write
  add = 0;
  do {
    if (si.i2c_start(add | I2C_WRITE)) {
      found = true;
      si.i2c_stop();
      Serial.print("Write:  0x");    
      if (add < 0x0F) Serial.print(0, HEX);  
      Serial.print(add+I2C_WRITE, HEX);
      Serial.print("  0x");
      if (add>>1 < 0x0F) Serial.print(0, HEX);
      Serial.println(add>>1, HEX);
    } else si.i2c_stop();
    si.i2c_stop();
    add += 2;
  } while (add);
  if (!found) Serial.println(F("No I2C device found."));
  Serial.println("Done\n\n");
  delay(1000);
}
