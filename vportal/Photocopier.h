#ifndef Photocopier_H
#define Photocopier_H

/* Static class for requesting blocks of data from flash memory */

#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#include <dataflash.h>
#define PRIMARY_BUFFER 1

#define BLOCK_SIZE 16

#define PAGE_SIZE 256 //bytes
#define PAGE_COUNT 2048 //4Mbit chip

#define BLOCKS_PER_PAGE 16

#define TOC_CHAPTER 0
#define TOC_SIZE 16 //Pages
#define CHAPTER_SIZE 4 //pages in a chapter/token


class Photocopier
{
  public:
    Photocopier();

    void init();
    void readBlock(int chapter, int block, uint8_t* buffer);
    void writeBlock(int chapter, int block, uint8_t* buffer);

  private:
    Dataflash dflash;
    uint8_t pageBuffer[PAGE_SIZE];


    void bufferPage(int page);
    void readFlash(int page, int len, uint8_t* buffer);
    void writeFlash(int page, int len, uint8_t* buffer);

};


#endif
