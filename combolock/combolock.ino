
#include <Bounce2.h>
#include <Encoder.h>
#include "RingCoder.h"

#include <EEPROM.h>
#define EEPROM_MAX 2047 //teensy 3.1

// Pin definitions - Encoder:
const int bPin = 0;  // Encoder B pin, D2 is external interrupt 0
const int aPin = 1;  // Encoder A pin, D3 is external interrupt 1
const int redPin = 3;  // Encoder's red LED - D5 is PWM enabled
const int bluPin = 4;  // Encoder's blue LED- D6 is PWM enabled
const int grnPin = 5;  // Encoder's green LED - D9 is PWM enabled
const int swhPin = 2;  // Encoder's switch pin

// Pin definitions - Shift registers:
const int datPin = 6;  // shift registers' SER pin
const int clrPin = 7;  // shift registers' srclr pin
const int clkPin = 8;  // Shift registers' srclk pin
const int latchPin = 9;  // Shift registers' rclk pin
const int enPin = 10;  // Shift registers' Output Enable pin

RingCoder ringcoder = RingCoder(bPin, aPin, redPin, bluPin, grnPin, swhPin, datPin, clrPin, clkPin, latchPin, enPin);
int lastPos = 0;

enum ledCounter {RED = 0, BLUE = 1, GREEN = 2, NONE = 3};
byte ledCount = RED;
byte ledValue[3] = {KNOB_LED_MAX, KNOB_LED_MAX, KNOB_LED_MAX};

void setup() {
  Serial.begin(9600);
  ringcoder.spin();
  ringcoder.spin();
  ringcoder.spin();
  //randomize();
}

void loop() {
  bool stateChanged = ringcoder.update();
  int state = ringcoder.button();
  int pos = ringcoder.readEncoder();

  if (stateChanged && state == HIGH) {
    ledCount = ++ledCount % (NONE + 1);

    if (ledCount == NONE) {
      ringcoder.writeEncoder(0);
      checkValues();
    } else {
      //set to the encoder value from the last time we adjusted that LED
      ringcoder.writeEncoder(ledValue[ledCount]);
      Serial.print("loading former value ");
      Serial.print(ringcoder.readEncoder());
      Serial.print(" From stored LED value ");
      Serial.print(ledValue[ledCount]);
      Serial.println("");
    }

    Serial.println("ledCount: " + String(ledCount));
  }

  if (ringcoder.moved() || stateChanged) {
    Serial.println("Encoder position: " + String(pos));

    if (ledCount != NONE) {  // Only update the LED if it's RED, GREEN or BLUE
      ledValue[ledCount] = pos * (KNOB_LED_MAX / LED_COUNT);
      Serial.println("Setting LED to " + String(ledValue[ledCount]));
      ringcoder.setKnobRgb(ledValue[RED], ledValue[GREEN], ledValue[BLUE]);
    }
    lastPos = pos;
  }
  if (ledCount != NONE) {
    ringcoder.ledRingFollower();  // Update the bar graph LED
  }
}

void checkValues() {
  int r = EEPROM.read(RED);
  int g = EEPROM.read(GREEN);
  int b = EEPROM.read(BLUE);

  if (r == ledValue[RED] &&
      g == ledValue[GREEN] &&
      b == ledValue[BLUE]) {
    Keyboard.print("Secret Word");
  } else {
    Serial.print("(");
    Serial.print(ledValue[RED]);
    Serial.print(",");
    Serial.print(ledValue[GREEN]);
    Serial.print(",");
    Serial.print(ledValue[BLUE]);
    Serial.print(")");
    Serial.println("");
    ringcoder.blink();
    ringcoder.blink();
    ringcoder.blink();
  }
}


