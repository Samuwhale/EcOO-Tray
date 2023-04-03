#include <LiquidCrystal.h>

#include <grove_two_rgb_led_matrix.h>
#include <FastLED.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"




#ifdef ARDUINO_SAMD_VARIANT_COMPLIANCE
#define SERIAL SerialUSB
#else
#define SERIAL Serial
#endif

int streakAmount = 5;

// Sets streak and updates LEDS accordingly
void setStreak(int streak) {
  streakAmount = streak;
  updateLeds();
}

// meatscore gaat van  0 (slecht) - 1 (best)
float meatScore = 0.5f; 

void setup() {

  // setupNFC();
  delay(1000);
  setupLeds();
  setupLcd();
  setupMatrix();
}

void loop() {
  // readNFC();
  updateMatrix();
}

// RFID / NFC
// DOCS: https://how2electronics.com/interfacing-pn532-nfc-rfid-module-with-arduino/



// LED MATRIX
#ifndef _BV
  #define _BV(bit) (1<<(bit))
#endif




static const uint8_t PROGMEM smile_bmp[] =
  { B00111100,
  B01000010,
  B10100101,
  B10000001,
  B10100101,
  B10011001,
  B01000010,
  B00111100 },
  neutral_bmp[] =
  { B00111100,
  B01000010,
  B10100101,
  B10000001,
  B10111101,
  B10000001,
  B01000010,
  B00111100 },
  frown_bmp[] =
  { B00111100,
  B01000010,
  B10100101,
  B10000001,
  B10011001,
  B10100101,
  B01000010,
  B00111100 };

// Als het niet werkt: 
Adafruit_LEDBackpack matrix = Adafruit_LEDBackpack();

uint8_t counter = 0;

void setupMatrix() {
  Serial.begin(9600);  
  matrix.begin(0x70);  // pass in the address
}

unsigned long currentTime;
int bitmapIndex = 0;
unsigned long lastBmpTime = 0;
int msToWait = 1000;

void updateMatrix(){
  currentTime = millis();

  if (currentTime - lastBmpTime > msToWait) {
    lastBmpTime = currentTime;
  // paint one LED per row. The HT16K33 internal memory looks like
  // a 8x16 bit matrix (8 rows, 16 columns)
  for (uint8_t i=0; i<8; i++) {
    // draw a diagonal row of pixels
    matrix.displaybuffer[i] = _BV((counter+i) % 16) | _BV((counter+i+8) % 16)  ;
  }
  // write the changes we just made to the display
  matrix.writeDisplay();
  counter++;
  if (counter >= 16) counter = 0;  }

}


// LED STRIP
// port 7
#define NUM_LEDS 41
#define LED_DATA_PIN 8
#define LED_BRIGHTNESS 128
#define LED_TYPE WS2811
#define COLOR_ORDER GRB

CRGB leds[NUM_LEDS];

void setupLeds() {
  FastLED.addLeds<LED_TYPE, LED_DATA_PIN, RGB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(LED_BRIGHTNESS);
  updateLeds();
}

void updateLeds() {
  int segmentSize = NUM_LEDS / 5;
  int currentColorIndex = 0;
  for (int i = 0; i < NUM_LEDS; i = i + 1) {
    leds[i] = CRGB::Black;
    if (i % segmentSize == 0) {
      currentColorIndex++;
    }

    if (currentColorIndex <= streakAmount) {
     switch (currentColorIndex) {
      case 1:
        leds[i] = CRGB::Red;
        break;
      case 2:
        leds[i] = CRGB::Blue;
        break;
      case 3:
        leds[i] = CRGB::Yellow;
        break;
      case 4:
        leds[i] = CRGB::Purple;
        break;
      case 5:
        leds[i] = CRGB::Green;
        break;
      }
    }
  }

  FastLED.show();
}

// LCD scherm
/* 
 * The circuit:
 * LCD RS pin to digital pin 12
 * LCD Enable pin to digital pin 11
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)
 */

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void showTextOnLcd(String textToDisplay) {
      lcd.clear();
      lcd.begin(16, 2);
      lcd.print(textToDisplay);
}

 void setupLcd() { 
   lcd.begin(16, 2);
   lcd.print("hello!");
 }

