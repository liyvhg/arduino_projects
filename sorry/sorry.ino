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
  int value = debouncer.read();

  if (value == LOW) {
    Serial.println(s(value));
    digitalWrite(LED_PIN, HIGH);
  } else {
    digitalWrite(LED_PIN, LOW);
  }
}

String s(int state) {
  return state == HIGH ? "HIGH" : "LOW";
}

String t(boolean state) {
  return state ? "True" : "False";
}


