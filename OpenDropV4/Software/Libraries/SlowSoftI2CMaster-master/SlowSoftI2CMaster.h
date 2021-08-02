/* Arduino Slow Software I2C Master 
   Copyright (c) 2017 Bernhard Nebel.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 3 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef SLOW_SOFT_I2C_MASTER_H
#define SLOW_SOFT_I2C_MASTER_H

#include <Arduino.h>
#include <inttypes.h>

#define I2C_READ 1
#define I2C_WRITE 0
#define DELAY 4 // usec delay
#define BUFFER_LENGTH 32
#define I2C_MAXWAIT 5000

class SlowSoftI2CMaster {
 public:
  SlowSoftI2CMaster(uint8_t sda, uint8_t scl);
  SlowSoftI2CMaster(uint8_t sda, uint8_t scl, bool internal_pullup);
  bool i2c_init(void);
  bool i2c_start(uint8_t addr);
  bool i2c_start_wait(uint8_t addr);
  bool i2c_rep_start(uint8_t addr);
  void i2c_stop(void);
  bool i2c_write(uint8_t value);
  uint8_t i2c_read(bool last);
  bool error;
  
 private:
  void setHigh(uint8_t pin);
  void setLow(uint8_t pin);
  uint8_t _sda;
  uint8_t _scl;
  bool _pullup;
};

#endif
