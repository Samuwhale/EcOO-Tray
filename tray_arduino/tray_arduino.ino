#include <grove_two_rgb_led_matrix.h>
#include <FastLED.h>


void setup() {
    setupMatrix();

    // Call displayNextEmoji() every displayInterval milliseconds
    setInterval(displayInterval, displayNextEmoji);
}

void loop() {
  // put your main code here, to run repeatedly:

}


// RFID 
// DOCS: https://how2electronics.com/interfacing-pn532-nfc-rfid-module-with-arduino/ 

// LED MATRIX
// DOCS: https://wiki.seeedstudio.com/Grove-RGB_LED_Matrix_w-Driver/ 
#ifdef ARDUINO_SAMD_VARIANT_COMPLIANCE
    #define SERIAL SerialUSB
#else
    #define SERIAL Serial
#endif

#define DISPLAY_COLOR    0X11

const int displayDuration = 5000; // milliseconds
const int displayInterval = 5000; // milliseconds

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

