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


// array of 10 names to display randomly on startup, normally the username and data would be obtained from reading the NFC chip. This just simulates a random user.
String realNames[10] = { "Alice", "Bob", "Charlie", "Dave", "Eve", "Frank", "Grace", "Heidi", "Ivan", "Julia" };
String userName = realNames[(int)random(10)];

static int streakAmount = 0; // the amount of days the user has eaten sustainable, yet again, this would normally be obtained from an actual user's NFC chip.
static int meatScore = 0; // the sustainability of the meal the user is currently eating. This too would normally be set while checking out at the register.


void setup() {
 userName = realNames[(int)random(10)];

  delay(2000);
  setupLeds();

  setupMatrix();

  drawSmiley();
  updateLcd();
  displayStreaks();
}

void loop() {
  //  randomizeValues();
}

// For evaluation, this function was written to randomize states (random streak, user, meat-score), but in the end it was not used.

// const unsigned long INTERVAL = 500;
// unsigned long randomizeTimer = millis();
// void randomizeValues() {
//   unsigned long currentTime = millis();
//   if (currentTime - randomizeTimer >= INTERVAL) {
//     randomizeTimer = currentTime;
//     setupLeds();
//     setupMatrix();

//     // set random values
//     setStreak((int)random(6));
//     setUsername(realNames[(int)random(10)]);
//     setMeatscore((int)random(3));
//     Serial.write("Setting Random");
//     Serial.println();

//   }
// }


/* LED MATRIX */
// The following section covers the functionality of the LED-matrix, in order to display smileys based on the sustainability (meatScore) of the current meal.


const uint8_t smile_bmp[] PROGMEM = {
  B00000000,
  B01000010,
  B01000010,
  B00000000,
  B01111110,
  B01000010,
  B00111100,
  B00000000,
};

const uint8_t mild_happy_bmp[] PROGMEM = {
  B00000000,
  B01000010,
  B01000010,
  B00000000,
  B00000000,
  B01000010,
  B00111100,
  B00000000
};

const uint8_t neutral_bmp[] PROGMEM = {
  B00000000,
  B01000010,
  B01000010,
  B00000000,
  B00000000,
  B01111110,
  B00000000,
  B00000000
};


Adafruit_LEDBackpack matrix = Adafruit_LEDBackpack();



void setupMatrix() {
  Serial.begin(9600);
  matrix.begin(0x70);  // pass in the address
}


void drawSmiley() {
  matrix.clear();

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


/* LED Strip */
// The following section covers the functionality of the LED-strip, in order to display the user's current streak.

#define LEDS_TO_LIGHT_UP 19
#define FIRST_SEGMENT_START 2
#define LED_DATA_PIN 8
#define LED_BRIGHTNESS 200
#define LED_TYPE WS2811
#define COLOR_ORDER GRB


// lights go from 2 - 16
CRGB leds[LEDS_TO_LIGHT_UP + FIRST_SEGMENT_START];

void setupLeds() {
  FastLED.addLeds<LED_TYPE, LED_DATA_PIN, RGB>(leds, LEDS_TO_LIGHT_UP + FIRST_SEGMENT_START).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(LED_BRIGHTNESS);
}

void displayStreaks() {


  int segmentSize = ceil(static_cast<double>(LEDS_TO_LIGHT_UP) / 5);
  int currentSegment = 0;
  for (int i; i <= LEDS_TO_LIGHT_UP; i = i + 1) {
    int j = i + FIRST_SEGMENT_START;
    leds[j] = CRGB::Black;
    if (i % segmentSize == 0) {
      currentSegment++;
    }

    if (currentSegment <= streakAmount) {
      switch (currentSegment) {
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


/* LCD Screen */
// The following section covers the functionality of the LCD display, to display feedback to the user based on its current meal.

 
// The circuit:
// LCD RS pin to digital pin 12
// LCD Enable pin to digital pin 11
// LCD D4 pin to digital pin 5
// LCD D5 pin to digital pin 4
// LCD D6 pin to digital pin 3
// LCD D7 pin to digital pin 2
// 10K resistor:
// ends to +5V and ground
// wiper to LCD VO pin (pin 3

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

unsigned long lcdTimer = 0;
void showTextOnLcd(String textToDisplay) {
  lcd.begin(16, 2);
  lcd.clear();

  int textLength = textToDisplay.length();
  int lcdLength = 16;

  int scrollIndex = 0;
  while (scrollIndex <= textLength - lcdLength) {
    unsigned long currentMillis = millis();
    if (currentMillis - lcdTimer >= 600) {
      lcdTimer = currentMillis;
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
