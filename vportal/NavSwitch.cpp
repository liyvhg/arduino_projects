
#include "NavSwitch.h"


NavSwitch::NavSwitch(int pinOne, int pinTwo, int pinTee) : pinOne (pinOne), pinTwo(pinTwo), pinTee(pinTee), oneBounce(), twoBounce(), teeBounce() {

}

void NavSwitch::init() {
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
  return oneBounce.update() || twoBounce.update() || teeBounce.update();
}

NavSwitch::NavDir NavSwitch::read() {
  if (oneBounce.read()) {
    return ONE;
  } else if (twoBounce.read()) {
    return TWO;
  } else if (teeBounce.read()) {
    return TEE;
  }
  return NONE;
}
