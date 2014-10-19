
//Includes
#include <Bounce2.h>
#include <SHA204.h>
#include <SHA204ReturnCodes.h>
#include <SHA204Definitions.h>
#include <SHA204SWI.h>


//Defines

#define MOVEMENT_STEP 20
#define MAX_LED 255
#define SERIAL_NUMBER_SIZE 9
#define COMPILED_AT __TIMESTAMP__

//Consts
const int sha204Pin = 10;

const int btnPin = 13;

const int rightPin = 14;
const int leftPin = 15;
const int downPin = 16;
const int upPin = 17;

const int whtPin = 18;
const int grnPin = 19;
const int redPin = 20;
const int bluPin = 21;

//Objects
Bounce button;
SHA204SWI sha204dev(sha204Pin);


volatile byte yPos;
volatile byte xPos;

//If using as mouse, up is negative
void isrUp( void ) {
  yPos = mod(yPos + MOVEMENT_STEP, MAX_LED);
  Mouse.move(0, -MOVEMENT_STEP);
}

void isrDown( void ) {
  yPos = mod(yPos - MOVEMENT_STEP, MAX_LED);
  Mouse.move(0, MOVEMENT_STEP);
}

void isrRight( void ) {
  xPos = mod(xPos + MOVEMENT_STEP, MAX_LED);
  Mouse.move(MOVEMENT_STEP, 0);
}

void isrLeft( void ) {
  xPos = mod(xPos - MOVEMENT_STEP, MAX_LED);
  Mouse.move(-MOVEMENT_STEP, 0);
}

void setupIsr() {
  // attachInterrupt(interrupt, function, mode)
  pinMode(upPin, INPUT);
  attachInterrupt(upPin, isrUp, CHANGE);
  pinMode(downPin, INPUT);
  attachInterrupt(downPin, isrDown, CHANGE);
  pinMode(rightPin, INPUT);
  attachInterrupt(rightPin, isrRight, CHANGE);
  pinMode(leftPin, INPUT);
  attachInterrupt(leftPin, isrLeft, CHANGE);
}

inline int mod(int i, int n) {
  return (i % n + n) % n;
}

void setupBRGWpins() {
  pinMode(bluPin, OUTPUT);
  pinMode(redPin, OUTPUT);
  pinMode(grnPin, OUTPUT);
  pinMode(whtPin, OUTPUT);
}

void setup() {
  Serial.begin(9600);
  setupBRGWpins();

  pinMode(btnPin, INPUT_PULLUP);
  button.attach(btnPin);
  button.interval(5);

  wakeupExample();
  setupIsr();
  Mouse.screenSize(2560, 1440);
}

void setBRGW(int b, int r, int g, int w) {
  analogWrite(bluPin, b);
  analogWrite(redPin, r);
  analogWrite(grnPin, g);
  analogWrite(whtPin, w);
}

byte returnValue;
uint8_t txBuffer[SHA204_CMD_SIZE_MIN] = {0};
uint8_t rxBuffer[SHA204_CMD_SIZE_MIN] = {0};
uint8_t randOut[RANDOM_RSP_SIZE] = {0};

#define MAX_SERIAL_BYTES 255
char inputBuffer[MAX_SERIAL_BYTES] = {0};
int bytesRead = 0;

void loop() {
  int stateChanged = button.update();
  int state = button.read();

  if (Serial.available()) {
    bytesRead = Serial.readBytes(inputBuffer, MAX_SERIAL_BYTES);
    if (bytesRead > 0) {
      Serial.println("So, what you're saying is, " + String(inputBuffer));
    }
  }

  if (stateChanged && state == LOW) {//button down
    Serial.println("Button pushed: " + String(xPos) + "," + String(yPos));
    returnValue = sha204dev.wakeup(txBuffer);
    if (returnValue != SHA204_SUCCESS) {
      return;
    }
    serialNumberExample();
    Mouse.click();

  }

  setBRGW(xPos, yPos, xPos, yPos);

}

void getRandom() {
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
