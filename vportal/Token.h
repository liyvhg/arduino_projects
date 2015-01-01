#ifndef Token_H
#define Token_H

#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#define PRIMARY_BUFFER 1

#define BLOCK_SIZE 16
#define BLOCK_COUNT 64

enum Element { MAGIC, EARTH, WATER, FIRE, TECH, UNDEAD, LIFE, AIR, DARK, LIGHT};

//All greater than 0x0F so they can be stored in high order bits
enum Type {
  PORTAL_MASTER = 0x10,
  TRAP = 0x20,
  MAGIC_ITEM = 0x30,
  LOCATION = 0x40,
  MINI = 0x50,
  REGULAR = 0x60
};

#define TYPE_MASK 0xF0
#define ELEMENT_MASK 0x0F

class Token
{
  public:
    Token(int libraryId);
    int read(int block, uint8_t* buffer);
    int write(int block, uint8_t* data);


    static void display(int libraryId, char* topline, char* bottomline);

  private:
    int libraryId;
    char name[BLOCK_SIZE-1];
    uint8_t elementAndType; //enums


};


#endif
