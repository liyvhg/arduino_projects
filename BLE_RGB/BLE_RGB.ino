#include <SPI.h>
#include <EEPROM.h>
#include <boards.h>
#include <RBL_nRF8001.h>
#include <Adafruit_NeoPixel.h>

#define NEO_PIN 9
#define LED_COUNT 24

Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_COUNT, NEO_PIN, NEO_GRB + NEO_KHZ800);
int R = 0;
int G = 0;
int B = 0;
int brightness = 255;

void setup() {  
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'  
  ble_begin();
}

void loop() {
  if (ble_connected()) {
    switch(ble_available()) {
      case 4:
        R = ble_read();
        G = ble_read();
        B = ble_read();
        brightness = ble_read();
        break;
      case 3:
        R = ble_read();
        G = ble_read();
        B = ble_read();
        break;
    }
    strip.setBrightness(brightness);
    colorWipe( strip.Color(R, G, B) );
  }
  ble_do_events();
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
  }
}
