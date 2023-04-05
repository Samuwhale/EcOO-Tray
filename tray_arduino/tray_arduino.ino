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

//interval at which random values are assigned
const unsigned long INTERVAL = 5000; 

// should be 0 - 5;
int streakAmount = 5;

// array of 10 'random' names
String realNames[10] = {"Alice", "Bob", "Charlie", "Dave", "Eve", "Frank", "Grace", "Heidi", "Ivan", "Julia"};


// Sets streak and updates LEDS accordingly
void setStreak(int streak) {
  streakAmount = streak;
  updateLeds();
}

// 0 = neutral, 1 = happy, 2 = mild-happy
int meatScore = 1;

String userName = "Tim";

void setUsername(String name) {
  userName = name;
  updateLcd();
}

void setMeatscore(int score) {
  meatScore = score;
  updateMatrix();
  updateLcd();
}

void setup() {
  delay(1000);
  setupLeds();

  setupMatrix();

  setStreak(5);
  setMeatscore(2);
}

void loop() {
 // randomizeValues();
}

void randomizeValues() {
  static unsigned long previousMillis = 0;
  unsigned long currentMillis = millis();
  
  
  if (currentMillis - previousMillis >= INTERVAL) {
    previousMillis = currentMillis;
    
    // set random values
    setStreak(random(6));
    setUsername(realNames[random(10)]);
    setMeatscore(random(3));
  }
}

// LED MATRIX



const uint8_t smile_bmp[] PROGMEM = { 
B00000000,
B01000010,
B01000010,
B00000000,
B01111110,
B01000010,
B00111100,
B00000000, };

const uint8_t mild_happy_bmp[] PROGMEM = { 
B00000000,
B01000010,
B01000010,
B00000000,
B00000000,
B01000010,
B00111100,
B00000000 };

const uint8_t neutral_bmp[] PROGMEM = { 
B00000000,
B01000010,
B01000010,
B00000000,
B00000000,
B01111110,
B00000000,
B00000000 };







Adafruit_LEDBackpack matrix = Adafruit_LEDBackpack();


void setupMatrix() {
  Serial.begin(9600);
  matrix.begin(0x70);  // pass in the address
}


void updateMatrix() {
  // paint one LED per row. 
  // flips the smiley to have correct rotation through j.
  for (uint8_t i = 0; i < 8; i++) {
    uint8_t j = 7 - i;
    if (meatScore == 0) matrix.displaybuffer[i] = neutral_bmp[j];
    else if (meatScore == 1) matrix.displaybuffer[i] = smile_bmp[j];
    else matrix.displaybuffer[i] = mild_happy_bmp[j];
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


// LED STRIP
// port 7
#define NUM_LEDS 16
#define LED_OFFSET 1
#define LED_DATA_PIN 8
#define LED_BRIGHTNESS 128
#define LED_TYPE WS2811
#define COLOR_ORDER GRB


// lights go from 2 - 16 
CRGB leds[NUM_LEDS];

void setupLeds() {
  FastLED.addLeds<LED_TYPE, LED_DATA_PIN, RGB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(LED_BRIGHTNESS);
}

void updateLeds() {
  int segmentSize = NUM_LEDS / 5;
  int currentColorIndex = 0;
  for (int i; i <= NUM_LEDS; i = i + 1) {
    int j = i + LED_OFFSET;
    leds[j] = CRGB::Black;
    if (i % segmentSize == 0) {
      currentColorIndex++;
    }

    if (currentColorIndex <= streakAmount) {
      switch (currentColorIndex) {
        case 1:
          leds[j] = CRGB::Red;
          break;
        case 2:
          leds[j] = CRGB::Blue;
          break;
        case 3:
          leds[j] = CRGB::Yellow;
          break;
        case 4:
          leds[j] = CRGB::Purple;
          break;
        case 5:
          leds[j] = CRGB::Green;
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
  if (meatScore == 0) showTextOnLcd("You've been eating a lot of meat, try to eat less!");
  else if (meatScore == 1) showTextOnLcd("You've been eating very sustainable, nice job!");
  else showTextOnLcd("You've been eating okay, but there's room for improvement!");
}

