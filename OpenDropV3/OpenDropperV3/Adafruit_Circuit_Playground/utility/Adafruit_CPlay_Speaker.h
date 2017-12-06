// Adafruit Circuit Playground speaker library
// by Phil Burgess / Paint Your Dragon.

#ifndef ADAFRUIT_CPLAY_SPEAKER_H
#define ADAFRUIT_CPLAY_SPEAKER_H

#include <Arduino.h>

#ifdef __AVR__
#else             // circuit playground express has nicer amp w/shutdown
  #define CPLAY_SPEAKER_SHUTDOWN 11
#endif

class Adafruit_CPlay_Speaker {
 public:
  Adafruit_CPlay_Speaker(void) { started = false; };
  void begin(void),
       end(void),
       set(uint8_t value),
       playSound(const uint8_t *data, uint32_t length, uint16_t sampleRate, boolean tenBit=false),
       say(const uint8_t *address);

  void enable(boolean e);
  void off(void) { enable(false); };
  void on(void) { enable(true); };

 private:
  boolean started;
};

#endif // ADAFRUIT_CPLAY_SPEAKER_H
