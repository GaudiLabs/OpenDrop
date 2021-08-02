// Sketch to explore 24AA1024 using SlowSoftI2CMaster

#include <SlowSoftI2CMaster.h>

SlowSoftI2CMaster si = SlowSoftI2CMaster(A4, A5, false);

#define EEPROMADDR 0xA6 // set by jumper (A0 and A1 = High)
#define MAXADDR 0x1FFFF
#define MAXTESTADDR 0x007FF

//------------------------------------------------------------------------------
/*
 * read one byte from address
 */
boolean readEEPROM(unsigned long address, uint8_t *byte) {
  // issue a start condition, send device address and write direction bit
  if (!si.i2c_start(EEPROMADDR | I2C_WRITE | (address&0x10000 ? 8 : 0) )) return false;

  // send the address
  if (!si.i2c_write((address>>8)&0xFF)) return false;
  if (!si.i2c_write(address&0xFF)) return false;

  // issue a repeated start condition, send device address and read direction bit
  if (!si.i2c_rep_start(EEPROMADDR | I2C_READ | (address&0x10000 ? 8 : 0) ))return false;

  *byte = si.i2c_read(true);

  si.i2c_stop();
  return true;
}
//------------------------------------------------------------------------------
/* 
 *burst read 
 */
boolean readBurstEEPROM(unsigned long start, unsigned long stop) {
  // does not handle the transition from 0x0FFFF to 0x10000
  // since we only use it for performance evaluation, we do not care!
  unsigned long addr = start;
  uint8_t byte;
  // issue a start condition, send device address and write direction bit
  if (!si.i2c_start(EEPROMADDR | I2C_WRITE | (addr&0x10000 ? 8 : 0) )) return false;

  // send the address
  if (!si.i2c_write((addr>>8)&0xFF)) return false;
  if (!si.i2c_write(addr&0xFF)) return false;

  // issue a repeated start condition, send device address and read direction bit
  if (!si.i2c_rep_start(EEPROMADDR | I2C_READ | (addr&0x10000 ? 8 : 0) ))return false;
  addr++;
  while (addr++ < stop) byte = si.i2c_read(false);
  byte = si.i2c_read(true);
  si.i2c_stop();
  return true;
}


//------------------------------------------------------------------------------

/*
 * write 1 byte to 'address' in eeprom
 */
boolean writeEEPROM(long unsigned address, uint8_t byte) {
  // issue a start condition, send device address and write direction bit
  if (!si.i2c_start(EEPROMADDR | I2C_WRITE | (address&0x10000 ? 8 : 0))) return false;

  // send the address
  if (!si.i2c_write((address>>8)&0xFF)) return false;
  if (!si.i2c_write(address&0xFF)) return false;


  // send data to EEPROM
  if (!si.i2c_write(byte)) return false;

  // issue a stop condition
  si.i2c_stop();

  delay(6);

  return true;
}

//------------------------------------------------------------------------------
/*
 * delete eeprom
 */
boolean deleteEEPROM(long unsigned from, unsigned long to, uint8_t byte, 
		     boolean poll) {

  unsigned long tempto, i;
  boolean firstpage = true;

  while (from <= to) {
    tempto = ((from/128)+1)*128-1;
    if (tempto > to) tempto = to;
    if (firstpage || !poll) {
      if (!si.i2c_start(EEPROMADDR | I2C_WRITE | (from&0x10000 ? 8 : 0))) 
	return false;
    } else si.i2c_start_wait(EEPROMADDR | I2C_WRITE | (from&0x10000 ? 8 : 0));
    // send the address
    if (!si.i2c_write((from>>8)&0xFF)) return false;
    if (!si.i2c_write(from&0xFF)) return false;
    
    
    // send data to EEPROM
    for (i=from; i<=tempto; i++)
      if (!si.i2c_write(byte)) return false;
    // issue a stop condition
    si.i2c_stop();

    // wait for ack again
    if (!poll) delay(6);

    from = tempto+1;
    firstpage = false;
  }
  return true;
}

//------------------------------------------------------------------------------
boolean performanceTest() {
  unsigned long eeaddr;
  unsigned long startmicros, endmicros;
  int avgtime;
  boolean OK = true;
  uint8_t byte;

  Serial.println(F("\nPerformance test:"));
  
  Serial.println(F("Sequential reads ..."));
  startmicros = micros();
  OK &= readBurstEEPROM(0,MAXTESTADDR);
  endmicros = micros();
  Serial.print(F("Time: "));
  avgtime = (endmicros-startmicros)/(MAXTESTADDR+1);
  Serial.print(avgtime);
  Serial.println(F(" micro secs/byte"));
  
  Serial.println(F("Random reads ..."));
  startmicros = micros();
  for (eeaddr = 0; eeaddr <= MAXTESTADDR; eeaddr++) 
    OK &= readEEPROM(eeaddr,&byte);
  endmicros = micros();
  Serial.print(F("Time: "));
  avgtime = (endmicros-startmicros)/(MAXTESTADDR+1);
  Serial.print(avgtime);
  Serial.println(F(" micro secs/byte"));

  Serial.println(F("Random writes ..."));
  startmicros = micros();
  for (eeaddr = 0; eeaddr <= MAXTESTADDR; eeaddr++) 
    OK &= writeEEPROM(eeaddr,0x55);
  endmicros = micros();
  Serial.print(F("Time: "));
  avgtime = (endmicros-startmicros)/(MAXTESTADDR+1);
  Serial.print(avgtime);
  Serial.println(F(" micro secs/byte"));
  
  Serial.println(F("Page writes with wait ..."));
  startmicros = micros();
  OK &= deleteEEPROM(0,MAXTESTADDR,0xFF,false);
  endmicros = micros();
  Serial.print(F("Time: "));
  avgtime = (endmicros-startmicros)/(MAXTESTADDR+1);
  Serial.print(avgtime);
  Serial.println(F(" micro secs/byte"));

  Serial.println(F("Page writes with poll ..."));
  startmicros = micros();
  OK &= deleteEEPROM(0,MAXTESTADDR,0xFF,true);
  endmicros = micros();
  Serial.print(F("Time: "));
  avgtime = (endmicros-startmicros)/(MAXTESTADDR+1);
  Serial.print(avgtime);
  Serial.println(F(" micro secs/byte"));
  
  return OK;
}
//------------------------------------------------------------------------------

unsigned long parseHex() {
  unsigned long result = 0L;
  char inp = '\0';
  byte val = 0;

  while (inp != '\r' && inp != '#') {
    while (!Serial.available());
    inp = Serial.read();
    if (inp == '\r' || inp == '#') break;
    if (inp >= 'a' && inp <= 'f')
      inp = inp -'a' + 'A';
    if ((inp >= '0' && inp <= '9') ||
	(inp >= 'A' && inp <= 'F')) {
      Serial.print(inp);
      if (inp >= '0' && inp <= '9') val = inp - '0';
      else val = inp - 'A' + 10;
      result = result * 16;
      result = result + val;
    }
  }
  Serial.println();
  return result;
}


void help (void) {
  Serial.println();
  Serial.println(F("r - read byte from address"));
  Serial.println(F("w - write byte to address"));
  Serial.println(F("d - delete from start address to end address"));  
  Serial.println(F("l - list memory range"));
  Serial.println(F("p - test performance"));
  Serial.println(F("h - help message"));
  Serial.println(F("Finish all numeric inputs with '#'"));
}

//------------------------------------------------------------------------------



void setup(void) {
  Serial.begin(115200);
  Serial.println(F("\n\nTest program for EEPROM 24AA1025 (SlowSoftI2CMaster)"));
  help();
}


void loop(void) {
  char cmd;
  uint8_t byte;
  boolean noterror;
  unsigned long addr, toaddr;

  while (!Serial.available());
  cmd = Serial.read();
  switch (cmd) {
  case 'r': Serial.print(F("Read from addr: "));
    addr = parseHex();
    Serial.println(F("Reading..."));
    noterror = readEEPROM(addr,&byte);
    Serial.print(addr,HEX);
    Serial.print(F(": "));
    if (byte < 0x10) Serial.print("0");
    Serial.println(byte,HEX);
    if (!noterror) Serial.println(F("Error while reading"));
    break;
  case 'w':
    Serial.print(F("Write to addr: "));
    addr = parseHex();
    Serial.print(F("Value: "));
    byte = parseHex();
    Serial.println(F("Writing..."));
    noterror = writeEEPROM(addr,byte);
    if (!noterror) Serial.println(F("Error while reading"));
    break;
  case 'd':
    Serial.print(F("Delete from addr: "));
    addr = parseHex();
    Serial.print(F("to addr: "));
    toaddr = parseHex();
    Serial.print(F("Value: "));
    byte = parseHex();
    Serial.print(F("Deleting ... "));
    noterror = deleteEEPROM(addr,toaddr,byte,false);
    Serial.println(F("...done"));
    if (!noterror) Serial.println(F("Error while deleting"));
    break;
  case 'l':
    Serial.print(F("List from addr: "));
    addr = parseHex();
    Serial.print(F("to addr: "));
    toaddr = parseHex();
    while (addr <= toaddr) {
      noterror = readEEPROM(addr,&byte);
      Serial.print(addr,HEX);
      Serial.print(F(": "));
      if (byte < 0x10) Serial.print("0");
      Serial.println(byte,HEX);
      if (!noterror) Serial.println(F("Error while reading"));
      addr++;
    }
    break;
  case 'p': 
    noterror = performanceTest();
    if (!noterror) Serial.println(F("Error while executing performance test"));
    break;
  case 'h':
    help();
    break;
  case '\r':
  case '\n':
  case ' ':
    break;
  default:
    Serial.println(F("Unknown command"));
    Serial.println();
    help();
  }
}
