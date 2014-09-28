
//Includes
#include <Bounce2.h>
#include <SHA204.h>
#include <SHA204ReturnCodes.h>
#include <SHA204Definitions.h>
#include <SHA204SWI.h>


//Defines

#define MAX_LED 255
#define SERIAL_NUMBER_SIZE 9

//Consts
const int sha204Pin = 18;

const int btnPin = 19;
const int whtPin = 20;
const int grnPin = 21;
const int redPin = 22;
const int bluPin = 23;

const int ledPin = 13;

//Objects
Bounce button;
SHA204SWI sha204dev(sha204Pin);

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
  while(!Serial);

  setupBRGWpins();
  setupTennsyLed();

  pinMode(btnPin, INPUT_PULLUP);
  button.attach(btnPin);
  button.interval(5);

  tennsyBlink();

  wakeupExample();
  serialNumberExample();

}

void setBRGW(int b, int r, int g, int w) {
  analogWrite(bluPin, b);
  analogWrite(redPin, r);
  analogWrite(grnPin, g);
  analogWrite(whtPin, w);
}

byte returnValue;
uint8_t txBuffer[SHA204_CMD_SIZE_MIN] = {0};
uint8_t randOut[RANDOM_RSP_SIZE] = {0};

void loop() {
  int stateChanged = button.update();
  int state = button.read();

  if (stateChanged && state == LOW) {//button down
    Serial.println("Button pushed");
    returnValue = sha204dev.random(txBuffer, randOut, RANDOM_NO_SEED_UPDATE);
    if (returnValue == SHA204_SUCCESS) {
      for (int i = 0; i < RANDOM_RSP_SIZE; i++) {
        Serial.print(randOut[i], HEX);
        Serial.print(" ");
      }
      Serial.println();
    } else {
      Serial.println("sha204dev.random() return: " + returnString(returnValue));
    }

  }

  int speed = 1000/MAX_LED;
  for (int i = 0; i < MAX_LED; i++) {
    setBRGW(0, 0, 0, i);
    delay(speed);
  }
  for (int i = MAX_LED - 1; i > 0; i--){
    setBRGW(0, 0, 0, i);
    delay(speed);
  }

}

void tennsyBlink() {
  digitalWrite(ledPin, HIGH);   // set the LED on
  delay(500);                  // wait for a second
  digitalWrite(ledPin, LOW);    // set the LED off
  delay(500);                  // wait for a second
}

byte wakeupExample() {
  uint8_t response[SHA204_RSP_SIZE_MIN];
  byte returnValue;

  returnValue = sha204dev.wakeup(response);
  if (returnValue == SHA204_SUCCESS) {
    for (int i = 0; i < SHA204_RSP_SIZE_MIN; i++) {
      Serial.print(response[i], HEX);
      Serial.print(" ");
    }
    Serial.println();
  } else {
    Serial.println("sha204dev.wakeup: " + returnString(returnValue));
  }

  return returnValue;
}

byte serialNumberExample() {
  byte returnValue = 0;
  uint8_t serialNumber[SERIAL_NUMBER_SIZE] = {0};


  Serial.println("Asking the SHA204's serial number. Response should be:");
  Serial.println("1 23 x x x x x x x EE");

  returnValue = sha204dev.serialNumber(serialNumber);
  if (returnValue == SHA204_SUCCESS) {
    for (int i = 0; i < SERIAL_NUMBER_SIZE; i++) {
      Serial.print(serialNumber[i], HEX);
      Serial.print(" ");
    }
    Serial.println();
  } else {
    Serial.println("sha204dev.serialNumber: " + returnString(returnValue));
  }


  return returnValue;
}

String s(int state) {
  return state == HIGH ? "HIGH" : "LOW";
}

String returnString(byte returnValue) {
  switch (returnValue) {
    case SHA204_SUCCESS:
      return "Success";
    case SHA204_RX_NO_RESPONSE:
      return "No response";
    default:
      return String(returnValue);
  }
}
