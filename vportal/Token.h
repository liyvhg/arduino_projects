#ifndef Token_H
#define Token_H

#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#define TOKEN_IMPORT

#include <dataflash.h>
#define PRIMARY_BUFFER 1
#define SECONDARY_BUFFER 2

#define LCD Serial1
#define LCD_MOVE 0xFE
#define LCD_TOP 0x80
#define LCD_BOTTOM 0xC0

#define BLOCK_SIZE 16
#define BLOCK_COUNT 64

#define PAGE_SIZE 256 //bytes
#define PAGE_COUNT 2048 //4Mbit chip

#define BLOCKS_PER_PAGE 16

#define TOC_CHAPTER 0
#define TOC_SIZE 16 //Pages
#define CHAPTER_SIZE 4 //pages in a chapter/token

enum Element { MAGIC, EARTH, WATER, FIRE, TECH, UNDEAD, LIFE, AIR, DARK, LIGHT};

//All greater than 0x0F so they can be stored in high order bits
enum Type {
  TRAP_MASTER = 0x10,
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
    void display();
    uint8_t type();
    uint8_t element();

#ifdef TOKEN_IMPORT
    static void import();
#endif

  private:
    Dataflash dflash;
    int libraryId;
    char name[BLOCK_SIZE];
    uint8_t elementAndType; //enums
    void readFlash(int block, uint8_t* buffer);
    void writeFlash(int block, uint8_t* buffer);

};


#endif
