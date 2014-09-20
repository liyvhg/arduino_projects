
#include <Bounce2.h>
#include <Encoder.h>
#include "RingCoder.h"


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

void setup() {
  Serial.begin(9600);
  ringcoder.blink();
  ringcoder.blink();
  ringcoder.blink();
}

void loop() {
  bool stateChanged = ringcoder.update();
  int state = ringcoder.button();
  int pos = ringcoder.readEncoder();

  if (stateChanged && state == HIGH) {
    ringcoder.spin_the_wheel();
  }
  int rgb = pos * KNOB_LED_MAX / LED_COUNT;
  ringcoder.setKnobRgb(rgb, rgb, rgb);

}

