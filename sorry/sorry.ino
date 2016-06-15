#include <Bounce2.h>
#include "password.h"

#define BUTTON_PIN 0
#define LED_PIN 13

Bounce debouncer = Bounce();

void setup() {
  Serial.begin(9600);

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  debouncer.attach(BUTTON_PIN);
  debouncer.interval(5); // interval in ms
}

void loop() {
  debouncer.update();

  if (debouncer.fell() ) {
    Keyboard.println(PASSWORD);
  }

  digitalWrite(LED_PIN, debouncer.read() == LOW ? HIGH : LOW);
}

String s(int state) {
  return state == HIGH ? "HIGH" : "LOW";
}

String t(boolean state) {
  return state ? "True" : "False";
}
