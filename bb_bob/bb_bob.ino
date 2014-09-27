
#include <Bounce2.h>

const int btnPin = 19;
const int atshaSdaPin= 7;
const int ledPin = 13;
Bounce button;

String s(int state) {
  return state == HIGH ? "HIGH" : "LOW";
}

void setup() {
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  pinMode(btnPin, INPUT_PULLUP);
  button.attach(btnPin);
  button.interval(5);

  Serial.println("Started");
}



void loop() {
  int stateChanged = button.update();
  int state = button.read();

  if (stateChanged && state == LOW) {
    Serial.println("Button pushed");
  }
}

void blink() {
  digitalWrite(ledPin, HIGH);   // set the LED on
  delay(500);                  // wait for a second
  digitalWrite(ledPin, LOW);    // set the LED off
  delay(500);                  // wait for a second
}
