// -*- c++ -*-

// --------------------------------------
// i2c_scanner (using SoftWire and SoftI2CMaster)
//
// Version 1
//    This program (or code that looks like it)
//    can be found in many places.
//    For example on the Arduino.cc forum.
//    The original author is not know.
// Version 2, Juni 2012, Using Arduino 1.0.1
//     Adapted to be as simple as possible by Arduino.cc user Krodal
// Version 3, Feb 26  2013
//    V3 by louarnold
// Version 4, March 3, 2013, Using Arduino 1.0.3
//    by Arduino.cc user Krodal.
//    Changes by louarnold removed.
//    Scanning addresses changed from 0...127 to 1...119,
//    according to the i2c scanner by Nick Gammon
//    http://www.gammon.com.au/forum/?id=10896
// Version 5, March 28, 2013
//    As version 4, but address scans now to 127.
//    A sensor seems to use address 120.
// Version 6, November 27, 2015.
//    Added waiting for the Leonardo serial communication.
// Version 7, October 1, 2016
//    Changed it to use the SoftWire library
// Version 8, August 5, 2017
//    Added third parameter for internal pullups
//
// This sketch tests the standard 7-bit addresses
// Devices with higher bit address might not be seen properly.
//

#include <SlowSoftWire.h>

SlowSoftWire Wire = SlowSoftWire(A4, A5, true);
     
void setup()
{
  Wire.begin();
  
  Serial.begin(19200);
  while (!Serial);             // Leonardo: wait for serial monitor
  Serial.println("\nI2C Scanner");
}


void loop()
{
  byte error, address;
  int nDevices;
  
  Serial.println(F("Scanning I2C bus (7-bit addresses) ..."));
  
  nDevices = 0;
  for(address = 1; address < 127; address++ )
    {
      // The i2c_scanner uses the return value of
      // the Write.endTransmisstion to see if
      // a device did acknowledge to the address.
      Wire.beginTransmission(address);
      error = Wire.endTransmission();
      
      if (error == 0)
        {
          Serial.print(F("I2C device found at address 0x"));
          if (address<16)
            Serial.print(F("0"));
          Serial.print(address,HEX);
          Serial.println(F("  !"));
	  
          nDevices++;
        }
      else if (error==4)
        {
          Serial.print(F("Unknow error at address 0x"));
          if (address<16)
            Serial.print("0");
          Serial.println(address,HEX);
        }    
    }
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else 
    Serial.println("done\n");
  
  delay(5000);           // wait 5 seconds for next scan
}

