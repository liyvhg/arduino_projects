#include <Bounce2.h>

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

  if (debouncer.fell() ){

    Keyboard.set_modifier(MODIFIERKEY_CTRL);
    Keyboard.send_now();

    Keyboard.set_modifier(MODIFIERKEY_CTRL | MODIFIERKEY_ALT);
    Keyboard.send_now();

    Keyboard.set_modifier(MODIFIERKEY_CTRL | MODIFIERKEY_ALT | MODIFIERKEY_GUI);
    Keyboard.send_now();

    Keyboard.set_key1(KEY_8);
    Keyboard.send_now();

    // release all the keys at the same instant
    Keyboard.set_modifier(0);
    Keyboard.set_key1(0);
    Keyboard.send_now();
  }

  digitalWrite(LED_PIN, debouncer.read() == LOW ? HIGH : LOW);

}

String s(int state) {
  return state == HIGH ? "HIGH" : "LOW";
}

String t(boolean state) {
  return state ? "True" : "False";
}


