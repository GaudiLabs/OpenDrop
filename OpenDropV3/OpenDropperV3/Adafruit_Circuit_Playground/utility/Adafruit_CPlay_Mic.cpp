// Adafruit Circuit Playground microphone library
// by Phil Burgess / Paint Your Dragon.
// Fast Fourier transform section is derived from
// ELM-ChaN FFT library (see comments in ffft.S).

#include <Arduino.h>
#include "Adafruit_CPlay_Mic.h"

#define DC_OFFSET       (1023 / 3)
#define NOISE_THRESHOLD 3

// -------------------------------------------------------------------------

// Reads ADC for given interval (in milliseconds, 1-65535), returns max
// deviation from DC_OFFSET (e.g. 0-341).  Uses ADC free-run mode w/polling.
// Any currently-installed ADC interrupt handler will be temporarily
// disabled while this runs.

int Adafruit_CPlay_Mic::peak(uint16_t ms) {
#ifdef __AVR__
  uint8_t  admux_save, adcsra_save, adcsrb_save, timsk0_save, channel;
  uint16_t i, nSamples;
  int16_t  adc, adcMin, adcMax;

  nSamples = (9615L * ms + 500) / 1000;
  if(!nSamples) nSamples = 1;

  channel     = analogPinToChannel(4); // Pin A4 to ADC channel
  admux_save  = ADMUX;                 // Save ADC config registers
  adcsra_save = ADCSRA;
  adcsrb_save = ADCSRB;

  // Init ADC free-run mode; f = ( 8MHz/prescaler ) / 13 cycles/conversion
  ADCSRA = 0;                          // Stop ADC interrupt, if any
  ADMUX  = _BV(REFS0) | channel;       // Aref=AVcc, channel sel, right-adj
  ADCSRB = 0;                          // Free run mode, no high MUX bit
  ADCSRA = _BV(ADEN)  |                // ADC enable
           _BV(ADSC)  |                // ADC start
           _BV(ADATE) |                // Auto trigger
           _BV(ADIF)  |                // Reset interrupt flag
           _BV(ADPS2) | _BV(ADPS1);    // 64:1 / 13 = 9615 Hz

  // ADC conversion-ready bit is polled for each sample rather than using
  // an interrupt; avoids contention with application or other library
  // using ADC ISR for other things (there can be only one) while still
  // providing the speed & precise timing of free-run mode (a loop of
  // analogRead() won't get you that).
  adcMin = 1023;
  adcMax = 0;
  for(i=0; i<nSamples; i++) {
    while(!(ADCSRA & _BV(ADIF)));      // Wait for ADC result
    adc     = ADC;                     // 0-1023
    ADCSRA |= _BV(ADIF);               // Clear bit
    if(adc < adcMin) adcMin = adc;
    if(adc > adcMax) adcMax = adc;
  }

  ADMUX  = admux_save;                 // Restore ADC config
  ADCSRB = adcsrb_save;
  ADCSRA = adcsra_save;
  (void)analogRead(A4);                // Purge residue from ADC register

  adcMin = abs(adcMin - DC_OFFSET);
  adcMax = abs(adcMax - DC_OFFSET);
  if(adcMin > DC_OFFSET) adcMin = DC_OFFSET;
  if(adcMax > DC_OFFSET) adcMax = DC_OFFSET;
  if(adcMin > adcMax)    adcMax = adcMin;

  return adcMax;
#else
  return 0;
#endif
}

// -------------------------------------------------------------------------

// Captures ADC audio samples at maximum speed supported by 32u4 (9615 Hz).
// Ostensibly for FFT code (below), but might have other uses.  Uses ADC
// free-run mode w/polling.  Any currently-installed ADC interrupt handler
// will be temporarily disabled while this runs.  No other interrupts are
// disabled; as long as interrupt handlers are minor (e.g. Timer/Counter 0
// handling of millis() and micros()), this isn't likely to lose readings.

void Adafruit_CPlay_Mic::capture(int16_t *buf, uint8_t nSamples) {
#ifdef __AVR__
  uint8_t admux_save, adcsra_save, adcsrb_save, timsk0_save, channel;
  int16_t adc;

  channel     = analogPinToChannel(4); // Pin A4 to ADC channel
  admux_save  = ADMUX;                 // Save ADC config registers
  adcsra_save = ADCSRA;
  adcsrb_save = ADCSRB;

  // Init ADC free-run mode; f = ( 8MHz/prescaler ) / 13 cycles/conversion 
  ADCSRA = 0;                          // Stop ADC interrupt, if any
  ADMUX  = _BV(REFS0) | channel;       // Aref=AVcc, channel sel, right-adj
  ADCSRB = 0;                          // Free run mode, no high MUX bit
  ADCSRA = _BV(ADEN)  |                // ADC enable
           _BV(ADSC)  |                // ADC start
           _BV(ADATE) |                // Auto trigger
           _BV(ADIF)  |                // Reset interrupt flag
           _BV(ADPS2) | _BV(ADPS1);    // 64:1 / 13 = 9615 Hz

  // ADC conversion-ready bit is polled for each sample rather than using
  // an interrupt; avoids contention with application or other library
  // using ADC ISR for other things (there can be only one) while still
  // providing the speed & precise timing of free-run mode (a loop of
  // analogRead() won't get you that).
  for(uint8_t i=0; i<nSamples; i++) {
    while(!(ADCSRA & _BV(ADIF)));      // Wait for ADC result
    adc     = ADC;
    ADCSRA |= _BV(ADIF);               // Clear bit
    // FFT requires signed inputs; ADC output is unsigned.  DC offset is
    // NOT 512 on Circuit Playground because it uses a 1.1V OpAmp input
    // as the midpoint, and may swing asymmetrically on the high side.
    // Sign-convert and then clip range to +/- DC_OFFSET.
    if(adc <= (DC_OFFSET - NOISE_THRESHOLD)) {
      adc  -= DC_OFFSET;
    } else if(adc >= (DC_OFFSET + NOISE_THRESHOLD)) {
      adc  -= DC_OFFSET;
      if(adc > (DC_OFFSET * 2)) adc = DC_OFFSET * 2;
    } else {
      adc   = 0; // Below noise threshold
    }
    buf[i]  = adc;
  }

  ADMUX  = admux_save;                 // Restore ADC config
  ADCSRB = adcsrb_save;
  ADCSRA = adcsra_save;
  (void)analogRead(A4);                // Purge residue from ADC register
#else
#endif
}

// -------------------------------------------------------------------------

// Performs one cycle of fast Fourier transform (FFT) with audio captured
// from mic on A4.  Output is 32 'bins,' each covering an equal range of
// frequencies from 0 to 4800 Hz (i.e. 0-150 Hz, 150-300 Hz, 300-450, etc).
// Needs about 450 bytes free RAM to operate.

typedef struct {
  int16_t r;
  int16_t i;
} complex_t;

extern "C" { // In ffft.S
  void fft_input(const int16_t *, complex_t *),
       fft_execute(complex_t *),
       fft_output(complex_t *, uint16_t *);
} 

void Adafruit_CPlay_Mic::fft(
 uint16_t *spectrum) {               // Spectrum output buffer, uint16_t[32]
  if(spectrum) {
    int16_t   capBuf[64];            // Audio capture buffer
    complex_t butterfly[64];         // FFT "butterfly" buffer

    capture(capBuf, 64);             // Collect mic data into capBuf
    fft_input(capBuf, butterfly);    // Samples -> complex #s
    fft_execute(butterfly);          // Process complex data
    fft_output(butterfly, spectrum); // Complex -> spectrum (32 bins)
  }
}
