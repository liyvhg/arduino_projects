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
  byte steps = 0;
} rotation;

struct Blink {
  unsigned long previousUpdateMs = 0;
  char speed = 0;
  byte count = 0xFF;
  uint32_t saved[LED_COUNT] = {0};
} blinky;

enum AimlessStyle {
  off,
  roulette,
  jump
};

//Because 'random' is too loaded
struct Aimless {
  AimlessStyle style = off;
  unsigned long previousUpdateMs = 0;
  char speed = 0;
  byte count = 0;
} aimless;

void setup() {  
  randomSeed(analogRead(0));

  ring.begin();
  ring.setBrightness(64);
  setOne(0, ring.Color(0xFF, 0x00, 0x00));
  setOne(8, ring.Color(0x00, 0xFF, 0x00));
  setOne(16, ring.Color(0x00, 0x00, 0xFF));  
  ring.show();

  ble_set_name("TeleLux");
  ble_begin();

  testing(2);
}

void testing(int i) {
  switch(i) {
    case 0:
      break;
    case 1:
      rotation.speed = 18;
      rotation.steps = 240;
      break;
    case 2:
      blinky.speed = 2;
      blinky.count = 0xFF - 2;
      break;
    case 3:
      aimless.style = jump;
      aimless.speed = 1;
      aimless.count = 15;
      break;
    case 4:       
      aimless.style = roulette;
      aimless.speed = 2;
      aimless.count = 18;
      break;
  }
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
          rotation.steps = ble_read();
          break;                 
        case 'B': //Blink
        case 'F': //Flash
          blinky.speed = ble_read();
          blinky.count = 0xFF - ble_read(); //See flash function comment
          break;
        case 'J': //Jump
          aimless.style = jump;
          aimless.speed = ble_read();
          aimless.count = ble_read();
          break;
        case 'G': //Roulette
          break; //NOT READY YET
          aimless.style = roulette;                    
          aimless.speed = ble_read();
          break;
      }
    }    
  }
  //Update animations
  if (rotate() || flash() || aim()) {
    ring.show();
  }
  ble_do_events();
}

void setOne(int i, uint32_t c) {
  ring.setPixelColor(i, c);
}

void setAll(uint32_t c) {
  for(int i = 0; i < ring.numPixels(); i++) {
    setOne(i, c);
  }
}

void shift(int n) {
  uint32_t colors[LED_COUNT] = {0};
  for(int i = 0; i < LED_COUNT; i++) {      
    colors[i] = ring.getPixelColor(i);      
  }

  for(int i = 0; i < LED_COUNT; i++) {     
    setOne(i, colors[pmod(i + n, LED_COUNT)]);
  }
}

/*
 * a 'step' is a single movement of the pixels.
 * Future improvements:
 *  a 'count' that represents x complete cycles
 */
bool rotate() {
  if (rotation.speed == 0) {
    return false; //Stopped
  }
  if (rotation.steps == 0) {
    rotation.speed = 0;
  }

  unsigned long interval = 1000 / abs(rotation.speed);
  char direction = rotation.speed / abs(rotation.speed); //+1 or -1
  unsigned long currentMillis = millis();
  if (currentMillis - rotation.previousUpdateMs >= interval) {
    rotation.previousUpdateMs = currentMillis;
    
    shift(direction);    
    rotation.steps--;
    return true;
  }
  return false;
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
bool flash() {
  if (blinky.count == 0xFF) {
    return false;
  }

  unsigned long interval = 1000 / blinky.speed;
  unsigned long currentMillis = millis();
  if (currentMillis - blinky.previousUpdateMs >= interval) {
    blinky.previousUpdateMs = currentMillis;
    
    if (blinky.count % 2 == 0) {
      for(int i = 0; i < LED_COUNT; i++) {
        setOne(i, blinky.saved[i]);
      }          
    } else {  
      //Save
      for(int i = 0; i < LED_COUNT; i++) {      
        blinky.saved[i] = ring.getPixelColor(i);
      }
      //Off
      setAll(colors.off);
    }

    blinky.count++;
    return true;
  }
  return false;
}


bool aim() {
  switch(aimless.style) {
    case off:
      break;
    case roulette:
      return spinRoulette();
    case jump:
      return jumpAround(); 
  }  
  return false;
}

bool spinRoulette() {
  unsigned long interval = 1000 / abs(aimless.speed);
  char direction = aimless.speed / abs(aimless.speed); //+1 or -1
  unsigned long currentMillis = millis();
  if (currentMillis - aimless.previousUpdateMs >= interval) {
    aimless.previousUpdateMs = currentMillis;
    shift(direction);
    
    if (aimless.count == 0) {
      aimless.speed--;
      aimless.count = aimless.speed;  
    }
    
    if (aimless.speed == 0) {
      aimless.style = off;
    }
    return true;
  }
  return false;
}


bool jumpAround() {
  unsigned long interval = 1000 / abs(aimless.speed);
  unsigned long currentMillis = millis();
  if (currentMillis - aimless.previousUpdateMs >= interval) {
    aimless.previousUpdateMs = currentMillis;
    shift(random(ring.numPixels()));
    aimless.count--;
 
    if (aimless.count == 0) {
      aimless.style = off;
    }
    return true;
  }
  return false;
}

inline int pmod(int i, int n) { //Module with always positive result.
  return (i % n + n) % n;
}
