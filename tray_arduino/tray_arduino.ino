#include <PN532_I2C.h>
#include <grove_two_rgb_led_matrix.h>
#include <FastLED.h>


#ifdef ARDUINO_SAMD_VARIANT_COMPLIANCE
    #define SERIAL SerialUSB
#else
    #define SERIAL Serial
#endif


void setup() {
    setupMatrix();
    setupNFC();


}

void loop() {
  readNFC();
  emojiTimer();


}


// RFID / NFC
// DOCS: https://how2electronics.com/interfacing-pn532-nfc-rfid-module-with-arduino/ 
PN532_I2C pn532_i2c(Wire);
NfcAdapter nfc = NfcAdapter(pn532_i2c);
String tagId = "None";
byte nuidPICC[4];

void setupNFC() {
    SERIAL.begin(115200);
    SERIAL.println("System initialized");
    nfc.begin();
}

void readNFC() {
      if (nfc.tagPresent()) {
        NfcTag tag = nfc.read();
        tag.print();
        tagId = tag.getUidString();
    }
}

// LED MATRIX
// DOCS: https://wiki.seeedstudio.com/Grove-RGB_LED_Matrix_w-Driver/ 


#define DISPLAY_COLOR    0X11

const int displayDuration = 5000; // milliseconds
const int displayInterval = 5000; // milliseconds
int displayTimer = 0;

void waitForMatrixReady() {
    delay(1000);
}

GroveTwoRGBLedMatrixClass matrix;
int currentDisplayIndex = 0;

void displayNextEmoji() {
    matrix.displayEmoji(currentDisplayIndex, displayDuration, true);
    currentDisplayIndex++;
    if (currentDisplayIndex >= 35) {
        currentDisplayIndex = 0;
    }
}

void emojiTimer() {
  displayTimer = displayTimer + millis();
  if (displayTimer > displayInterval) {
    displayTimer = 0;
    displayNextEmoji();
  }
}

void setupMatrix() {
    Wire.begin();
    SERIAL.begin(115200);
    waitForMatrixReady();
    uint16_t VID = 0;
    VID = matrix.getDeviceVID();
    if (VID != 0x2886) {
        SERIAL.println("Can not detect led matrix!!!");
        while (1);
    }
    SERIAL.println("Matrix init success!!!");
}

