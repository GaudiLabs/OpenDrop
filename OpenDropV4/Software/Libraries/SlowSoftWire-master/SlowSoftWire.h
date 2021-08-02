/* Wire-like wrapper for SlowSoftI2CMaster */

#ifndef SLOW_SOFT_WIRE
#define SLOW_SOFT_WIRE
#include <SlowSoftI2CMaster.h>



class SlowSoftWire : public Stream
{
private:
  uint8_t rxBuffer[BUFFER_LENGTH];
  uint8_t rxBufferIndex;
  uint8_t rxBufferLength;
  uint8_t transmitting;
  uint8_t error;
  SlowSoftI2CMaster si2c;
public:
  SlowSoftWire(uint8_t sda, uint8_t scl);
  SlowSoftWire(uint8_t sda, uint8_t scl, bool internal_pullup);
  void begin(void);
  void end(void);
  void setClock(uint32_t _);
  void beginTransmission(uint8_t address);
  void beginTransmission(int address);
  uint8_t endTransmission(uint8_t sendStop);
  uint8_t endTransmission(void);
  size_t write(uint8_t data);
  size_t write(const uint8_t *data, size_t quantity);
  uint8_t requestFrom(uint8_t address, uint8_t quantity,
		      uint32_t iaddress, uint8_t isize, uint8_t sendStop);
  uint8_t requestFrom(uint8_t address, uint8_t quantity, uint8_t sendStop);
  uint8_t requestFrom(int address, int quantity, int sendStop);
  uint8_t requestFrom(uint8_t address, uint8_t quantity);
  uint8_t requestFrom(int address, int quantity);
  int available(void);
  int read(void);
  int peek(void);
  void flush(void);
  inline size_t write(unsigned long n) { return write((uint8_t)n); }
  inline size_t write(long n) { return write((uint8_t)n); }
  inline size_t write(unsigned int n) { return write((uint8_t)n); }
  inline size_t write(int n) { return write((uint8_t)n); }
  using Print::write;
};

#endif
