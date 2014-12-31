#ifndef Token_H
#define Token_H

#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#include <dataflash.h>
#define PRIMARY_BUFFER 1

#define BLOCK_SIZE 16
#define BLOCK_COUNT 64

#define PAGE_SIZE 264 //bytes
#define PAGE_DATA_SIZE 256
#define PAGE_COUNT 2048 //4Mbit chip

#define BLOCKS_PER_PAGE 16


enum Element { MAGIC, EARTH, WATER, FIRE, TECH, UNDEAD, LIFE, AIR, DARK, LIGHT};

//Might try assigning these to high order bits of a bite
enum Type { PORTAL_MASTER, TRAP, MAGIC_ITEM, LOCATION, MINI, REGULAR};

class Token
{
  public:
    Token(Dataflash *dflash, int libraryId);
    int read(int block, uint8_t* buffer);
    int write(int block, uint8_t* data);


    static void import(int libraryId, char *name, uint8_t elementAndType, uint8_t* data);

  private:
    int libraryId;
    Dataflash *dflash;
    uint8_t pageBuffer[PAGE_SIZE];

    char name[BLOCK_SIZE];
    uint8_t elementAndType; //enums


    void readFlash(int page, int len, uint8_t* buffer);
    void writeFlash(int page, int len, uint8_t* buffer);
};


#endif
