#include <FastLED.h>

#include <PN532_I2C.h>
#include <grove_two_rgb_led_matrix.h>
#include <FastLED.h>



#ifdef ARDUINO_SAMD_VARIANT_COMPLIANCE
    #define SERIAL SerialUSB
#else
    #define SERIAL Serial
#endif


void setup() {

    setupNFC();


}

void loop() {
  readNFC();


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
// 


// LED STRIP
// port 7 


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



