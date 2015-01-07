#ifndef VirtualPortal_h
#define VirtualPortal_h

#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#define BLE_ATTRIBUTE_MAX_VALUE_LENGTH 20
#define LCD Serial1

#include "Token.h"

class VirtualPortal
{
  public:
    VirtualPortal();

    int respondTo(uint8_t *message, uint8_t *response);
    bool loadToken(Token *t);
    uint8_t light(); //get current light value

  private:
    int sequence;
    uint8_t lightVal;
    Token *characterToken;

    int activate(uint8_t* message, uint8_t* response);
    int color(uint8_t* message);
    int jay(uint8_t* response);
    int light(uint8_t* message);
    int query(uint8_t* message, uint8_t* response);
    int reset(uint8_t* response);
    int status(uint8_t* response);
    int write(uint8_t* response);

    void printCommand(bool incoming, const uint8_t* command);
    //void printHex(String prefix, const unsigned char* buffer, int len, String suffix);

};

#endif

