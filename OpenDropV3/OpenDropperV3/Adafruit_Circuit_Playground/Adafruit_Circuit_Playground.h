/*------------------------------------------------------------------------
  This file is part of the Adafruit Dot Star library.

  Adafruit Dot Star is free software: you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public License
  as published by the Free Software Foundation, either version 3 of
  the License, or (at your option) any later version.

  Adafruit Dot Star is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with DotStar.  If not, see <http://www.gnu.org/licenses/>.
  ------------------------------------------------------------------------*/

#ifndef _ADAFRUIT_CIRCUITPLAYGROUND_H_
#define _ADAFRUIT_CIRCUITPLAYGROUND_H_

#include <Arduino.h>
#include "utility/Adafruit_CPlay_NeoPixel.h"
#include "utility/Adafruit_CPlay_LIS3DH.h"
#include "utility/Adafruit_CPlay_Mic.h"
#include "utility/Adafruit_CPlay_Speaker.h"
#include "utility/CP_Firmata.h"

#ifdef __AVR__ // Circuit Playground 'classic'
  #include "utility/CPlay_CapacitiveSensor.h"
#else
  #include "utility/Adafruit_CPlay_FreeTouch.h"
  #include "utility/IRLibCPE.h"
#endif

#ifndef NOT_AN_INTERRUPT // Not defined in Arduino 1.0.5
  #define NOT_AN_INTERRUPT -1
#endif

#ifdef __AVR__ // Circuit Playground 'classic'
 #define CPLAY_CAPSENSE_SHARED   30
 #define CPLAY_REDLED           13
 #define CPLAY_NEOPIXELPIN      17
 #define CPLAY_SLIDESWITCHPIN   21
 #define CPLAY_LEFTBUTTON        4
 #define CPLAY_RIGHTBUTTON      19
 #define CPLAY_LIGHTSENSOR      A5
 #define CPLAY_THERMISTORPIN    A0
 #define CPLAY_SOUNDSENSOR      A4
 #define CPLAY_BUZZER            5
 #define CPLAY_LIS3DH_CS         8
 #define CPLAY_LIS3DH_INTERRUPT  7
 #define CPLAY_LIS3DH_ADDRESS    0x18
#else // Circuit Playground Express
 #define CPLAY_LEFTBUTTON        4
 #define CPLAY_RIGHTBUTTON       5
 #define CPLAY_SLIDESWITCHPIN    7
 #define CPLAY_NEOPIXELPIN       8
 #define CPLAY_REDLED           13
 #define CPLAY_IR_EMITTER       25
 #define CPLAY_IR_RECEIVER      26
 #define CPLAY_BUZZER           A0
 #define CPLAY_LIGHTSENSOR      A8
 #define CPLAY_THERMISTORPIN    A9
 #define CPLAY_SOUNDSENSOR      A4 // TBD I2S
 #define CPLAY_LIS3DH_CS        -1 // I2C
 #define CPLAY_LIS3DH_INTERRUPT 27
 #define CPLAY_LIS3DH_ADDRESS   0x19
#endif

#define SERIESRESISTOR 10000
// resistance at 25 degrees C
#define THERMISTORNOMINAL 10000
// temp. for nominal resistance (almost always 25 C)
#define TEMPERATURENOMINAL 25

// The beta coefficient of the thermistor (usually 3000-4000)
#define BCOEFFICIENT 3380
// the value of the 'other' resistor

// Configuration to tune the color sensing logic:
#define LIGHT_SETTLE_MS 100  // Amount of time (in milliseconds) to wait between
                             // changing the pixel color and reading the light
                             // sensor.


class Adafruit_CircuitPlayground {
 public:
  boolean begin(uint8_t brightness=20);

  Adafruit_CPlay_NeoPixel strip;
  Adafruit_CPlay_LIS3DH lis;
  Adafruit_CPlay_Mic mic;
  Adafruit_CPlay_Speaker speaker;

#ifdef __AVR__ // Circuit Playground 'classic'
  CPlay_CapacitiveSensor cap[8];
#else
  Adafruit_CPlay_FreeTouch     cap[7];
  IRrecvPCI              irReceiver;
  IRdecode               irDecoder;
  IRsend                 irSend;
#endif

  boolean slideSwitch(void);
  void redLED(boolean v);
  void playTone(uint16_t freq, uint16_t time, boolean wait=true);
  boolean leftButton(void);
  boolean rightButton(void);
  uint16_t lightSensor(void);
  uint16_t soundSensor(void);
  float temperature(void);
  float temperatureF(void);

  uint16_t readCap(uint8_t p, uint8_t samples=10);

  // Accelerometer
  float motionX(void);
  float motionY(void);
  float motionZ(void);
  void setAccelRange(lis3dh_range_t range) { lis.setRange(range); }
  void setAccelTap(uint8_t c, uint8_t clickthresh) 
    { lis.setClick(c, clickthresh, 10, 20, 255); }
  uint8_t getAccelTap(void) { return (lis.getClick() >> 8) & 0x3; }


  // neopixels
  void clearPixels(void) { strip.clear(); strip.show(); }
  void setPixelColor(uint8_t p, uint32_t c) {strip.setPixelColor(p, c); strip.show();}
  void setPixelColor(uint8_t p, uint8_t r, uint8_t g, uint8_t b) {strip.setPixelColor(p, r, g, b); strip.show();}
  void setBrightness(uint16_t b){strip.setBrightness(b);}

  uint32_t colorWheel(uint8_t x);

  // Basic RGB color sensing with the light sensor and nearby neopixel.
  // Both functions do the same thing and just differ in how they return the
  // result, either as explicit RGB bytes or a 24-bit RGB color value.
  void senseColor(uint8_t& red, uint8_t& green, uint8_t& blue);
  uint32_t senseColor() {
    // Use the individual color component color sense function and then recombine
    // tbe components into a 24-bit color value.
    uint8_t red, green, blue;
    senseColor(red, green, blue);
    return ((uint32_t)red << 16) | ((uint32_t)green << 8) | blue;
  }

  boolean isExpress(void);

 private:


};


extern Adafruit_CircuitPlayground CircuitPlayground;

#endif
