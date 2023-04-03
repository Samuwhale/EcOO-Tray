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

int streakAmount = 3;

// Sets streak and updates LEDS accordingly
void setStreak(int streak) {
  streakAmount = streak;
  updateLeds();
}

// meatscore gaat van  0 (slecht) - 1 (best)
float meatScore = 0.7f;

String userName = "Tim";

void setUsername(String name) {
  userName = name;
  updateLcd();
}

void setMeatscore(float score) {
  meatScore = score;
  updateMatrix();
  updateLcd();
}

void setup() {
  // setupNFC();
  delay(1000);
  setupLeds();

  setupMatrix();

  setStreak(3);
  setMeatscore(0.3);
  // updateLcd();
  updateLeds();
}

void loop() {
  // readNFC();
}

// RFID / NFC
// DOCS: https://how2electronics.com/interfacing-pn532-nfc-rfid-module-with-arduino/



// LED MATRIX



const uint8_t smile_bmp[] PROGMEM = { B00111100,
                                      B01000010,
                                      B10100101,
                                      B10000001,
                                      B10100101,
                                      B10011001,
                                      B01000010,
                                      B00111100 };

const uint8_t neutral_bmp[] PROGMEM = { B00111100,
                                        B01000010,
                                        B10100101,
                                        B10000001,
                                        B10111101,
                                        B10000001,
                                        B01000010,
                                        B00111100 };

const uint8_t frown_bmp[] PROGMEM = { B00111100,
                                      B01000010,
                                      B10100101,
                                      B10000001,
                                      B10011001,
                                      B10100101,
                                      B01000010,
                                      B00111100 };





Adafruit_LEDBackpack matrix = Adafruit_LEDBackpack();


void setupMatrix() {
  Serial.begin(9600);
  matrix.begin(0x70);  // pass in the address
}


void updateMatrix() {
  // paint one LED per row. The HT16K33 internal memory looks like
  // a 8x16 bit matrix (8 rows, 16 columns)
  for (uint8_t i = 0; i < 8; i++) {
    if (meatScore < 0.4f) matrix.displaybuffer[i] = frown_bmp[i];
    else if (meatScore > 0.6f) matrix.displaybuffer[i] = smile_bmp[i];
    else matrix.displaybuffer[i] = neutral_bmp[i];
  }

  // Apply shifting/orientation fixes
  uint16_t displaybuffertemp[8];
  memcpy(displaybuffertemp, matrix.displaybuffer, sizeof(displaybuffertemp));

  // // Shift data to correct row
  // for (uint8_t i = 0; i < 8; i++) {
  //   uint8_t tempbuffer3 = displaybuffertemp[i] & 0xFF;
  //   displaybuffertemp[i] &= 0xFF00;
  //   displaybuffertemp[i] |= (tempbuffer3 << 1) | (tempbuffer3 >> 7);
  // }

  // Shift data to correct row
  uint16_t tempbuffer2 = displaybuffertemp[3];
  displaybuffertemp[3] = displaybuffertemp[5];
  displaybuffertemp[5] = tempbuffer2;

  // Mirror data
  for (uint8_t i = 0; i < 8; i++) {
    uint8_t tempbuffer = displaybuffertemp[i] & 0xFF;
    displaybuffertemp[i] &= 0xFF00;
    for (uint8_t k = 0; k < 8; k++) {
      if (tempbuffer & (0x01 << k)) {
        displaybuffertemp[i] |= 0x01 << (7 - k);
      }
    }
  }

  memcpy(matrix.displaybuffer, displaybuffertemp, sizeof(displaybuffertemp));

  // write the changes we just made to the display
  matrix.writeDisplay();
}


// Adafruit_8x8matrix matrix = Adafruit_8x8matrix();

// const int colors[3] = { LED_GREEN, LED_YELLOW, LED_RED };

// int count = 0;

// void setupMatrix() {
//   Serial.begin(9600);
//   matrix.begin(0x70);  // pass in the address
// }

// unsigned long currentTime;
// unsigned long lastBmpTime = 0;
// int msToWait = 400;

// void updateMatrix() {
//   currentTime = millis();

//   if (currentTime - lastBmpTime > msToWait) {
//     lastBmpTime = currentTime;
//     matrix.clear();

//     for (int x = 0; x < 8; x++) {
//       for (int y = 0; y < 8; y++) {
//         matrix.clear();
//         matrix.drawPixel(x, y, LED_ON);
//         matrix.writeDisplay();
//         delay(400);
//       }
//     }





//     // matrix.writeDisplay();

//     // matrix.drawBitmap(0, 0, smile_bmp, 8, 8, LED_ON);

//     matrix.writeDisplay();
//   }
// }



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
  lcd.begin(16, 2);
  lcd.clear();

  int textLength = textToDisplay.length();
  int lcdLength = 16;
  unsigned long previousMillis = 0;
  int scrollIndex = 0;
  while (scrollIndex <= textLength - lcdLength) {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= 600) {
      previousMillis = currentMillis;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Hi, " + userName);
      lcd.setCursor(0, 1);
      lcd.print(textToDisplay.substring(scrollIndex, scrollIndex + lcdLength));
      scrollIndex++;
    }
  }
}

void updateLcd() {
  if (meatScore < 0.4f) showTextOnLcd("You've been eating a lot of meat, try to eat less!");
  else if (meatScore > 0.6f) showTextOnLcd("You've been eating very sustainable, nice job!");
  else showTextOnLcd("You've been eating okay, but there's room for improvement!");
}

