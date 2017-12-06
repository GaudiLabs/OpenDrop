// This demo is based on the vumeter demo in the Adafruit Circuit Playground library.
#ifndef VUMETERDEMO_H
#define VUMETERDEMO_H
#include <math.h>

#include "Demo.h"

#define MIC_PIN         A4  // Microphone is attached to this analog pin (A4 for circuit playground)
#define SAMPLE_WINDOW   10  // Sample window for average level
#define PEAK_HANG       24  // Time of pause before peak dot falls
#define PEAK_FALL        4  // Rate of falling peak dot
#define INPUT_FLOOR     10  // Lower range of analogRead input
//#define INPUT_CEILING  500  // Max range of analogRead input, the lower the value the more sensitive (1023 = max)

static int inputCeilings[] = { 500, 1023, 250 };

static byte peak = 16;        // Peak level of column; used for falling dots
static unsigned int sample;
static byte dotCount = 0;     //Frame counter for peak dot
static byte dotHangCount = 0; //Frame counter for holding peak dot

//Used to draw a line between two points of a given color
static void drawLine(uint8_t from, uint8_t to, uint32_t c) {
  uint8_t fromTemp;
  if (from > to) {
    fromTemp = from;
    from = to;
    to = fromTemp;
  }
  for(int i=from; i<=to; i++){
    CircuitPlayground.strip.setPixelColor(i, c);
  }
}


static float fscale( float originalMin, float originalMax, float newBegin, float
newEnd, float inputValue, float curve){

  float OriginalRange = 0;
  float NewRange = 0;
  float zeroRefCurVal = 0;
  float normalizedCurVal = 0;
  float rangedValue = 0;
  boolean invFlag = 0;


  // condition curve parameter
  // limit range

  if (curve > 10) curve = 10;
  if (curve < -10) curve = -10;

  curve = (curve * -.1) ; // - invert and scale - this seems more intuitive - postive numbers give more weight to high end on output 
  curve = pow(10, curve); // convert linear scale into lograthimic exponent for other pow function

  /*
   Serial.println(curve * 100, DEC);   // multply by 100 to preserve resolution  
   Serial.println(); 
   */

  // Check for out of range inputValues
  if (inputValue < originalMin) {
    inputValue = originalMin;
  }
  if (inputValue > originalMax) {
    inputValue = originalMax;
  }

  // Zero Refference the values
  OriginalRange = originalMax - originalMin;

  if (newEnd > newBegin){ 
    NewRange = newEnd - newBegin;
  }
  else
  {
    NewRange = newBegin - newEnd; 
    invFlag = 1;
  }

  zeroRefCurVal = inputValue - originalMin;
  normalizedCurVal  =  zeroRefCurVal / OriginalRange;   // normalize to 0 - 1 float

  // Check for originalMin > originalMax  - the math for all other cases i.e. negative numbers seems to work out fine 
  if (originalMin > originalMax ) {
    return 0;
  }

  if (invFlag == 0){
    rangedValue =  (pow(normalizedCurVal, curve) * NewRange) + newBegin;

  }
  else     // invert the ranges
  {   
    rangedValue =  newBegin - (pow(normalizedCurVal, curve) * NewRange); 
  }

  return rangedValue;
}


class VUMeterDemo: public Demo {
public:
  VUMeterDemo() { currentCeiling = 0; }
  ~VUMeterDemo() {}

  virtual void loop() {
    int numPixels = CircuitPlayground.strip.numPixels();
    unsigned long startMillis= millis();  // Start of sample window
    float peakToPeak = 0;   // peak-to-peak level
  
    unsigned int signalMax = 0;
    unsigned int signalMin = 1023;
    unsigned int c, y;
  
    // collect data for length of sample window (in mS)
    while (millis() - startMillis < SAMPLE_WINDOW)
    {
      sample = analogRead(MIC_PIN);
      if (sample < 1024)  // toss out spurious readings
      {
        if (sample > signalMax)
        {
          signalMax = sample;  // save just the max levels
        }
        else if (sample < signalMin)
        {
          signalMin = sample;  // save just the min levels
        }
      }
    }
    peakToPeak = signalMax - signalMin;  // max - min = peak-peak amplitude
   
    // Serial.println(peakToPeak);
  
    //Fill the strip with rainbow gradient
    for (int i=0;i<=numPixels-1;i++){
      CircuitPlayground.strip.setPixelColor(i,CircuitPlayground.colorWheel(map(i,0,numPixels-1,30,150)));
    }
  
    //Scale the input logarithmically instead of linearly
    c = fscale(INPUT_FLOOR, inputCeilings[currentCeiling], numPixels, 0, peakToPeak, 2);
  
    // Turn off pixels that are below volume threshold.
    if(c < peak) {
      peak = c;        // Keep dot on top
      dotHangCount = 0;    // make the dot hang before falling
    }
    if (c <= numPixels) { // Fill partial column with off pixels
      drawLine(numPixels, numPixels-c, CircuitPlayground.strip.Color(0, 0, 0));
    }
  
    // Set the peak dot to match the rainbow gradient
    y = numPixels - peak;
    CircuitPlayground.strip.setPixelColor(y-1,CircuitPlayground.colorWheel(map(y,0,numPixels-1,30,150)));
    CircuitPlayground.strip.show();
  
    // Frame based peak dot animation
    if(dotHangCount > PEAK_HANG) { //Peak pause length
      if(++dotCount >= PEAK_FALL) { //Fall rate 
        peak++;
        dotCount = 0;
      }
    } 
    else {
      dotHangCount++; 
    }  
  }

  virtual void modePress() {
    // Ratchet through different input ceiling values.
    currentCeiling += 1;
    if (currentCeiling >= (sizeof(inputCeilings)/sizeof(int))) {
      currentCeiling = 0;
    }
  }

private:
  int currentCeiling;
};

#endif
