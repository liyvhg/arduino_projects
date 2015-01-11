
#include "NavSwitch.h"


NavSwitch::NavSwitch(int pinC, int pinTwo, int pinTee, int pinOne) : 
  pinC(pinC), 
  pinOne (pinOne), 
  pinTwo(pinTwo), 
  pinTee(pinTee), 
  oneBounce(), 
  twoBounce(), 
  teeBounce() {

}

void NavSwitch::init() {
  //Cheating and using pinC as a ground
  pinMode(pinC, OUTPUT);
  digitalWrite(pinC, LOW);

  pinMode(pinOne, INPUT_PULLUP);
  oneBounce.attach(pinOne);
  oneBounce.interval(5);

  pinMode(pinTwo, INPUT_PULLUP);
  twoBounce.attach(pinTwo);
  twoBounce.interval(5);

  pinMode(pinTee, INPUT_PULLUP);
  teeBounce.attach(pinTee);
  teeBounce.interval(5);

}

int NavSwitch::update() {
  uint8_t one = oneBounce.update();
  uint8_t two = twoBounce.update();
  uint8_t tee = teeBounce.update();
  //update() calls need to be made outside of ||'ing to prevent short circuiting
  return one || two || tee;
}

NavSwitch::NavDir NavSwitch::read() {
  if (oneBounce.read() == LOW) {
    return ONE;
  }
  if (twoBounce.read() == LOW) {
    return TWO;
  }
  if (teeBounce.read() == LOW) {
    return TEE;
  }
  return NONE;
}
