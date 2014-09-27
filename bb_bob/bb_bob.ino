
#include <Bounce2.h>

const int btnPin = 19;

const int whtPin = 20;
const int grnPin = 21;
const int redPin = 22;
const int bluPin = 23;

const int atshaSdaPin= 7;
const int ledPin = 13;
Bounce button;

String s(int state) {
  return state == HIGH ? "HIGH" : "LOW";
}

void setupBRGWpins() {
  pinMode(bluPin, OUTPUT);
  pinMode(redPin, OUTPUT);
  pinMode(grnPin, OUTPUT);
  pinMode(whtPin, OUTPUT);
}


void setupTennsyLed() {
  pinMode(ledPin, OUTPUT);
}

void setup() {
  Serial.begin(9600);
  setupBRGWpins();
  setupTennsyLed();

  pinMode(btnPin, INPUT_PULLUP);
  button.attach(btnPin);
  button.interval(5);
  tennsyBlink();
  Serial.println("Started");
}

void setBRGW(int b, int r, int g, int w) {
  analogWrite(bluPin, b);
  analogWrite(redPin, r);
  analogWrite(grnPin, g);
  analogWrite(whtPin, w);
}

int red = 0;
int green = 0;
int blue = 0;
int white = 0;

#define MAX 255

void loop() {
  int stateChanged = button.update();
  int state = button.read();

  red = (red + 1) % MAX;
  if (red == 0) {
    green = (green + 1) % MAX;
  }
  if (green == 0) {
    blue = (blue + 1) % MAX;
  }
  if (blue == 0) {
    white = (white + 1) % MAX;
  }
  if (white == 0) {
    delay(10);
  }

  if (stateChanged && state == LOW) {//button down
    Serial.println("Button pushed");
    red = green = blue = white = 0;
  }

  if (stateChanged && state == HIGH) {//button up

  }

  setBRGW(blue, red, green, white);
}

void tennsyBlink() {
  digitalWrite(ledPin, HIGH);   // set the LED on
  delay(500);                  // wait for a second
  digitalWrite(ledPin, LOW);    // set the LED off
  delay(500);                  // wait for a second
}
