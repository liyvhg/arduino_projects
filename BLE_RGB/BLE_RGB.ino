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

struct Colors {
  uint32_t off = ring.Color(0, 0, 0);
  uint32_t white = ring.Color(0xff, 0xff, 0xff);
} colors;

struct Rotation {
  unsigned long previousUpdateMs = 0;
  char speed = 0; //sign indicates direction
  byte count = 0;
} rotation;

struct Blink {
  unsigned long previousUpdateMs = 0;
  char speed = 0;
  byte count = 0xFF;
  uint32_t saved[LED_COUNT] = {0};
} blinky;

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
                 
         case 'B': //Blink
          blinky.speed = ble_read();
          blinky.count = 0xFF - ble_read();
          break;
      }
    }    
  }
  //Update animations
  rotate();
  flash();
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
  if (currentMillis - rotation.previousUpdateMs >= interval) {
    rotation.previousUpdateMs = currentMillis;
    rotation.count--;

    //Save off the existing colors to prevent loss
    uint32_t colors[LED_COUNT] = {0};
    for(int i = 0; i < LED_COUNT; i++) {      
      colors[i] = ring.getPixelColor(i);      
    }

    for(int i = 0; i < LED_COUNT; i++) {     
      ring.setPixelColor(i, colors[pmod(i + direction, LED_COUNT)]);
    }

    ring.show();
  }
}


/*
 * 0 = no op
 * 1 = off
 * 2 = off, then on
 * 3 = off, then on, then off
 */
/*
 * The speed is the amout of time in each state.  The count is 0xFF - the intended value in order to make the math work out for the final state
 * Future improvements:
 *  Color for alternate state
 *  Second 'speed' for the alternate state
 */
void flash() {
  if (blinky.count == 0xFF) {
    return;
  }

  unsigned long interval = 1000 / blinky.speed;
  unsigned long currentMillis = millis();
  if (currentMillis - blinky.previousUpdateMs >= interval) {
    blinky.previousUpdateMs = currentMillis;
    
    if (blinky.count % 2 == 0) {
      for(int i = 0; i < LED_COUNT; i++) {     
        ring.setPixelColor(i, blinky.saved[i]);
      }
    } else {  
      //Save
      for(int i = 0; i < LED_COUNT; i++) {      
        blinky.saved[i] = ring.getPixelColor(i);
      }
      //Off
      for(int i = 0; i < LED_COUNT; i++) {     
        ring.setPixelColor(i, colors.off);
      }
    }

    blinky.count++;
    ring.show();
  }
}

inline int pmod(int i, int n) { //Module with always positive result.
  return (i % n + n) % n;
}
