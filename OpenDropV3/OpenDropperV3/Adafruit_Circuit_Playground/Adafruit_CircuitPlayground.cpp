#include <Adafruit_Circuit_Playground.h>

boolean Adafruit_CircuitPlayground::begin(uint8_t brightness) {
  pinMode(CPLAY_REDLED, OUTPUT);
  pinMode(CPLAY_BUZZER, OUTPUT);
#ifdef __AVR__
  pinMode(CPLAY_CAPSENSE_SHARED, OUTPUT);
  pinMode(CPLAY_LEFTBUTTON, INPUT);
  pinMode(CPLAY_RIGHTBUTTON, INPUT);
  pinMode(CPLAY_SLIDESWITCHPIN, INPUT);
#else // Circuit Playground Express
  pinMode(CPLAY_LEFTBUTTON, INPUT_PULLDOWN);
  pinMode(CPLAY_RIGHTBUTTON, INPUT_PULLDOWN);
  pinMode(CPLAY_SLIDESWITCHPIN, INPUT_PULLUP);
  irReceiver=IRrecvPCI(CPLAY_IR_RECEIVER);
  irDecoder=IRdecode();
#endif


  strip = Adafruit_CPlay_NeoPixel();
  strip.updateType(NEO_GRB + NEO_KHZ800);
  strip.updateLength(10);
  strip.setPin(CPLAY_NEOPIXELPIN);

  lis = Adafruit_CPlay_LIS3DH(CPLAY_LIS3DH_CS);
  mic = Adafruit_CPlay_Mic();

  speaker.begin();

  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  strip.setBrightness(brightness);

#ifdef __AVR__
  cap[0] = CPlay_CapacitiveSensor(CPLAY_CAPSENSE_SHARED, 0);
  cap[1] = CPlay_CapacitiveSensor(CPLAY_CAPSENSE_SHARED, 1);
  cap[2] = CPlay_CapacitiveSensor(CPLAY_CAPSENSE_SHARED, 2);
  cap[3] = CPlay_CapacitiveSensor(CPLAY_CAPSENSE_SHARED, 3);
  cap[4] = CPlay_CapacitiveSensor(CPLAY_CAPSENSE_SHARED, 6);
  cap[5] = CPlay_CapacitiveSensor(CPLAY_CAPSENSE_SHARED, 9);
  cap[6] = CPlay_CapacitiveSensor(CPLAY_CAPSENSE_SHARED, 10);
  cap[7] = CPlay_CapacitiveSensor(CPLAY_CAPSENSE_SHARED, 12);
#else // Circuit Playground Express // Circuit Playground Express
  for(int i=0; i<7; i++) {
    cap[i] = Adafruit_CPlay_FreeTouch(A1+i, OVERSAMPLE_4, RESISTOR_50K, FREQ_MODE_NONE);
    if (! cap[i].begin()) return false;
  }
#endif

  return lis.begin(CPLAY_LIS3DH_ADDRESS);
}

uint16_t Adafruit_CircuitPlayground::readCap(uint8_t p, uint8_t samples) {
#ifdef __AVR__  // Circuit Playground Classic
  switch (p) {
    case 0:    return cap[0].capacitiveSensor(samples);
    case 1:    return cap[1].capacitiveSensor(samples);
    case 2:    return cap[2].capacitiveSensor(samples);
    case 3:    return cap[3].capacitiveSensor(samples);
    case 6:    return cap[4].capacitiveSensor(samples);
    case 9:    return cap[5].capacitiveSensor(samples);
    case 10:   return cap[6].capacitiveSensor(samples);
    case 12:   return cap[7].capacitiveSensor(samples);
    default:   return 0;
  }
#else // Circuit Playground Express // Circuit Playground Express
  // analog pins r ez!
  if ((p >= A1) && (p <= A7)) {
    return cap[p - A1].measure();
  }
  // oof digital pins
  switch (p) {
    case 0:    return cap[A6 - A1].measure();
    case 1:    return cap[A7 - A1].measure();
    case 2:    return cap[A5 - A1].measure();
    case 3:    return cap[A4 - A1].measure();
    case 6:    return cap[A1 - A1].measure();
    case 9:    return cap[A2 - A1].measure();
    case 10:   return cap[A3 - A1].measure();
    default:   return 0;
  }
#endif
}

// just turn on/off the red #13 LED
void Adafruit_CircuitPlayground::redLED(boolean v) {
  digitalWrite(CPLAY_REDLED, v);
}

// just read the slide switch
boolean Adafruit_CircuitPlayground::slideSwitch(void) {
  return digitalRead(CPLAY_SLIDESWITCHPIN);
}

// just read the left button
boolean Adafruit_CircuitPlayground::leftButton(void) {
  return digitalRead(CPLAY_LEFTBUTTON);
}

// just read the right button
boolean Adafruit_CircuitPlayground::rightButton(void) {
  return digitalRead(CPLAY_RIGHTBUTTON);
}

void Adafruit_CircuitPlayground::playTone(uint16_t freq, uint16_t time, boolean wait) {
  tone(CPLAY_BUZZER, freq, time);
  if (wait) delay(time);
}

uint16_t Adafruit_CircuitPlayground::lightSensor(void) {
  return analogRead(CPLAY_LIGHTSENSOR);
}

uint16_t Adafruit_CircuitPlayground::soundSensor(void) {
  return analogRead(CPLAY_SOUNDSENSOR);
}

float Adafruit_CircuitPlayground::motionX(void) {
  sensors_event_t event;
  CircuitPlayground.lis.getEvent(&event);
  return event.acceleration.x;
}

float Adafruit_CircuitPlayground::motionY(void) {
  sensors_event_t event;
  CircuitPlayground.lis.getEvent(&event);
  return event.acceleration.y;
}

float Adafruit_CircuitPlayground::motionZ(void) {
  sensors_event_t event;
  CircuitPlayground.lis.getEvent(&event);
  return event.acceleration.z;
}

float Adafruit_CircuitPlayground::temperature(void) {
   // Thermistor test
  float reading;

  reading = analogRead(CPLAY_THERMISTORPIN);

  //Serial.print("Thermistor reading: "); Serial.println(reading);

  // convert the value to resistance
  reading = ((1023.0 * SERIESRESISTOR) / reading);
  reading -= SERIESRESISTOR;

  //Serial.print("Thermistor resistance: "); Serial.println(reading);

  float steinhart;
  steinhart = reading / THERMISTORNOMINAL;     // (R/Ro)
  steinhart = log(steinhart);                  // ln(R/Ro)
  steinhart /= BCOEFFICIENT;                   // 1/B * ln(R/Ro)
  steinhart += 1.0 / (TEMPERATURENOMINAL + 273.15); // + (1/To)
  steinhart = 1.0 / steinhart;                 // Invert
  steinhart -= 273.15;                         // convert to C

  return steinhart;
}

// Get the temperature in degrees Fahrenheit
float Adafruit_CircuitPlayground::temperatureF(void) {
  float tempF = CircuitPlayground.temperature() * 1.8 + 32;
  return tempF;
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Adafruit_CircuitPlayground::colorWheel(uint8_t WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

void Adafruit_CircuitPlayground::senseColor(uint8_t& red, uint8_t& green, uint8_t& blue) {
  // Save the current pixel brightness so it can later be restored.  Then bump
  // the brightness to max to make sure the LED is as bright as possible for
  // the color readings.
  uint8_t old_brightness = strip.getBrightness();
  strip.setBrightness(255);
  // Set pixel 1 (next to the light sensor) to full red, green, blue
  // color and grab a light sensor reading.  Make sure to wait a bit
  // after changing pixel colors to let the light sensor change
  // resistance!
  setPixelColor(1, 255, 0, 0);  // Red
  delay(LIGHT_SETTLE_MS);
  uint16_t raw_red = lightSensor();
  setPixelColor(1, 0, 255, 0);  // Green
  delay(LIGHT_SETTLE_MS);
  uint16_t raw_green = lightSensor();
  setPixelColor(1, 0, 0, 255);  // Blue
  delay(LIGHT_SETTLE_MS);
  uint16_t raw_blue = lightSensor();
  // Turn off the pixel and restore brightness, we're done with readings.
  setPixelColor(1, 0);
  strip.setBrightness(old_brightness);
  // Now scale down each of the raw readings to be within
  // 0 to 255.  Remember each sensor reading is from the ADC
  // which has 10 bits of resolution (0 to 1023), so dividing
  // by 4 will change the range from 0-1023 to 0-255.  Also
  // use the min function to clamp the value to 255 at most (just
  // to prevent overflow from 255.xx to 0).
  red = min(255, raw_red/4);
  green = min(255, raw_green/4);
  blue = min(255, raw_blue/4);
}

// Returns true if Circuit Playground Express, false if "classic"
boolean Adafruit_CircuitPlayground::isExpress(void) {
#ifdef __AVR__
  return false;
#else
  return true;
#endif
}

// instantiate static
Adafruit_CircuitPlayground CircuitPlayground;
