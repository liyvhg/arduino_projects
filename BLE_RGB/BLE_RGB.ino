#include <SPI.h>
#include <EEPROM.h>
#include <boards.h>
#include <RBL_nRF8001.h>
#include <Adafruit_NeoPixel.h>

#define NEO_PIN 8
#define LED_COUNT 24

Adafruit_NeoPixel ring = Adafruit_NeoPixel(LED_COUNT, NEO_PIN, NEO_GRB + NEO_KHZ800);

struct RGB {
  byte r;
  byte g;
  byte b;
};

struct Rotation {
  char speed = 0; //sign indicates direction
  byte count = 0;
} rotation;

struct Blink {
  char speed = 0;
  byte count = 0; //even ends on, odd ends off
};

enum RStyle {
  roulette,
  jump 
};

struct Random {
  RStyle type;
  byte endIndex;
};

void setup() {  
  ring.begin();
  ring.setBrightness(64);
  setOne(0, ring.Color(0xFF, 0x00, 0x00));
  setOne(8, ring.Color(0x00, 0xFF, 0x00));
  setOne(16, ring.Color(0x00, 0x00, 0xFF));  
  ring.show();

  ble_set_name("TeleLux");
  ble_begin();
}

void loop() {
  int R = 0;
  int G = 0;
  int B = 0;
  int index = 0;

  if (ble_connected()) {
    if(ble_available()) {
      char command = ble_read();
      switch(command) {
        case 'O': //Set One
          index = ble_read();
          R = ble_read();
          G = ble_read();
          B = ble_read();
          setOne(index, ring.Color(R, G, B));
          break;
        case 'A': //Set All                
          R = ble_read();        
          G = ble_read();
          B = ble_read();
          setAll(ring.Color(R, G, B));
          break;
        case 'R': //Rotate
          rotation.speed = ble_read();
          rotation.count = ble_read();
          break;
      }
    }    
  }
  rotate(); //Always called, may be a no-op
  ble_do_events();
}

void setOne(int i, uint32_t c) {
  ring.setPixelColor(i, c);      
  ring.show();
}

void setAll(uint32_t c) {
  for(int i=0; i<ring.numPixels(); i++) {
    setOne(i, c);
  }
}

unsigned long previousUpdateMs;

void rotate() {
  if (rotation.speed == 0) {
    return; //Stopped
  }
  if (rotation.count == 0) {
    rotation.speed = 0;
  }

  unsigned long interval = 1000 / abs(rotation.speed);
  char direction = rotation.speed / abs(rotation.speed); //+1 or -1
  unsigned long currentMillis = millis();
  if (currentMillis - previousUpdateMs >= interval) {
    previousUpdateMs = currentMillis;
    rotation.count--;
    int max = ring.numPixels(); //pre-compute

    //Save off the existing colors to prevent loss
    uint32_t colors[LED_COUNT] = {0};
    for(int i = 0; i < max; i++) {      
      colors[i] = ring.getPixelColor(i);      
    }

    for(int i = 0; i < max; i++) {     
      ring.setPixelColor(i, colors[pmod(i + direction,max)]);
    }

    ring.show();
  }
}

inline int pmod(int i, int n) { //Module with always positive result.
  return (i % n + n) % n;
}
