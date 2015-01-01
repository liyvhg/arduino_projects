#ifndef NavSwitch_H
#define NavSwitch_H

#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#include <Bounce2.h>

class NavSwitch
{
  public:
    enum NavDir { NONE, ONE, TWO, TEE };

    NavSwitch(int pinTwo, int pinTee, int pinOne);
    void init();
    int update();
    NavSwitch::NavDir read();

  private:
    int pinOne, pinTwo, pinTee;
    Bounce oneBounce, twoBounce, teeBounce;

};


#endif

