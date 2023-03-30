
#include <grove_two_rgb_led_matrix.h>
#include <FastLED.h>



#ifdef ARDUINO_SAMD_VARIANT_COMPLIANCE
#define SERIAL SerialUSB
#else
#define SERIAL Serial
#endif


void setup() {

  // setupNFC();
  setupLeds();
}

void loop() {
  // readNFC();

}


// RFID / NFC
// DOCS: https://how2electronics.com/interfacing-pn532-nfc-rfid-module-with-arduino/
// PN532_I2C pn532_i2c(Wire);
// NfcAdapter nfc = NfcAdapter(pn532_i2c);
// String tagId = "None";
// byte nuidPICC[4];

// void setupNFC() {
//   SERIAL.begin(115200);
//   SERIAL.println("System initialized");
//   nfc.begin();
// }

// void readNFC() {
//   if (nfc.tagPresent()) {
//     NfcTag tag = nfc.read();
//     tag.print();
//     tagId = tag.getUidString();
//   }
// }


// LED MATRIX
//


// LED STRIP
// port 7
#define NUM_LEDS 41
#define LED_DATA_PIN 8
#define LED_BRIGHTNESS 64
#define LED_TYPE WS2811
#define COLOR_ORDER GRB

CRGB leds[NUM_LEDS];

void setupLeds() {
  delay(2000);
  FastLED.addLeds<LED_TYPE, LED_DATA_PIN, RGB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(LED_BRIGHTNESS);
  fiveColorStrip();
}

void fiveColorStrip() {
  int segmentSize = NUM_LEDS / 5;
  int currentColorIndex = 0;
  for(int i = 0; i < NUM_LEDS; i = i+1) {
    if (i % segmentSize == 0) {
      currentColorIndex++;
    }
    switch(currentColorIndex) {
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
