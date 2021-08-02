/* Arduino Slow Software I2C Master 
   Copyright (c) 2017 Bernhard Nebel.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public License
   as published by the Free Software Foundation; either version 3 of
   the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301
   USA
*/

#include <SlowSoftI2CMaster.h>

SlowSoftI2CMaster::SlowSoftI2CMaster(uint8_t sda, uint8_t scl) {
  _sda = sda;
  _scl = scl;
  _pullup = false;
}

SlowSoftI2CMaster::SlowSoftI2CMaster(uint8_t sda, uint8_t scl, bool pullup) {
  _sda = sda;
  _scl = scl;
  _pullup = pullup;
}
// Init function. Needs to be called once in the beginning.
// Returns false if SDA or SCL are low, which probably means 
// a I2C bus lockup or that the lines are not pulled up.
bool SlowSoftI2CMaster::i2c_init(void) {
  digitalWrite(_sda, LOW);
  digitalWrite(_scl, LOW);
  setHigh(_sda);
  setHigh(_scl);
  if (digitalRead(_sda) == LOW || digitalRead(_scl) == LOW) return false;
  return true;
}

// Start transfer function: <addr> is the 8-bit I2C address (including the R/W
// bit). 
// Return: true if the slave replies with an "acknowledge", false otherwise
bool SlowSoftI2CMaster::i2c_start(uint8_t addr) {
  setLow(_sda);
  delayMicroseconds(DELAY);
  setLow(_scl);
  return i2c_write(addr);
}

// Try to start transfer until an ACK is returned
bool SlowSoftI2CMaster::i2c_start_wait(uint8_t addr) {
  long retry = I2C_MAXWAIT;
  while (!i2c_start(addr)) {
    i2c_stop();
    if (--retry == 0) return false;
  }
  return true;
}

// Repeated start function: After having claimed the bus with a start condition,
// you can address another or the same chip again without an intervening 
// stop condition.
// Return: true if the slave replies with an "acknowledge", false otherwise
bool SlowSoftI2CMaster::i2c_rep_start(uint8_t addr) {
  setHigh(_sda);
  setHigh(_scl);
  delayMicroseconds(DELAY);
  return i2c_start(addr);
}

// Issue a stop condition, freeing the bus.
void SlowSoftI2CMaster::i2c_stop(void) {
  setLow(_sda);
  delayMicroseconds(DELAY);
  setHigh(_scl);
  delayMicroseconds(DELAY);
  setHigh(_sda);
  delayMicroseconds(DELAY);
}

// Write one byte to the slave chip that had been addressed
// by the previous start call. <value> is the byte to be sent.
// Return: true if the slave replies with an "acknowledge", false otherwise
bool SlowSoftI2CMaster::i2c_write(uint8_t value) {
  for (uint8_t curr = 0X80; curr != 0; curr >>= 1) {
    if (curr & value) setHigh(_sda); else  setLow(_sda); 
    setHigh(_scl);
    delayMicroseconds(DELAY);
    setLow(_scl);
  }
  // get Ack or Nak
  setHigh(_sda);
  setHigh(_scl);
  delayMicroseconds(DELAY/2);
  uint8_t ack = digitalRead(_sda);
  setLow(_scl);
  delayMicroseconds(DELAY/2);  
  setLow(_sda);
  return ack == 0;
}

// Read one byte. If <last> is true, we send a NAK after having received 
// the byte in order to terminate the read sequence. 
uint8_t SlowSoftI2CMaster::i2c_read(bool last) {
  uint8_t b = 0;
  setHigh(_sda);
  for (uint8_t i = 0; i < 8; i++) {
    b <<= 1;
    delayMicroseconds(DELAY);
    setHigh(_scl);
    if (digitalRead(_sda)) b |= 1;
    setLow(_scl);
  }
  if (last) setHigh(_sda); else setLow(_sda);
  setHigh(_scl);
  delayMicroseconds(DELAY/2);
  setLow(_scl);
  delayMicroseconds(DELAY/2);  
  setLow(_sda);
  return b;
}

void SlowSoftI2CMaster::setLow(uint8_t pin) {
    noInterrupts();
    if (_pullup) 
      digitalWrite(pin, LOW);
    pinMode(pin, OUTPUT);
    interrupts();
}


void SlowSoftI2CMaster::setHigh(uint8_t pin) {
    noInterrupts();
    if (_pullup) 
      pinMode(pin, INPUT_PULLUP);
    else
      pinMode(pin, INPUT);
    interrupts();
}

