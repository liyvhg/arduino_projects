#ifndef VirtualPortal_h
#define VirtualPortal_h

#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#define BLE_ATTRIBUTE_MAX_VALUE_LENGTH 20

#include "Token.h"

class VirtualPortal
{
  public:
    VirtualPortal(int lightPin);

    int respondTo(uint8_t *message, uint8_t *response);
    bool loadToken();

  private:
    int lightPin, sequence;
    Token *characterToken;

    int activate(uint8_t* message, uint8_t* response);
    int color(uint8_t* message);
    int jay(uint8_t* response);
    int light(uint8_t* message);
    int query(uint8_t* message, uint8_t* response);
    int reset(uint8_t* response);
    int status(uint8_t* response);
    int write(uint8_t* response);

};

#endif

