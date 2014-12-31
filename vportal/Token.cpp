

#include "Token.h"

Token::Token(Dataflash *dflash, int libraryId) : dflash(dflash), libraryId(libraryId) {
  //Read in character name and save
  //The pages are 256 + 8 bytes, so I store the character name at the end of the first 2 pages
  readFlash(0, PAGE_SIZE, pageBuffer);
  memcpy(name, pageBuffer + PAGE_DATA_SIZE, 8);
}

int Token::read(int block, uint8_t* buffer) {
  if (block > BLOCK_COUNT) {
    return 0;
  }

  //TODO: Clean this up and abstract
  int page = (libraryId * 4) + (block / BLOCKS_PER_PAGE);
  int offset = (block % BLOCKS_PER_PAGE) * BLOCK_SIZE;

  //Go get the data
  readFlash(page, PAGE_SIZE, pageBuffer);

  memcpy(buffer, pageBuffer+offset, BLOCK_SIZE);

  return BLOCK_SIZE;
}

int Token::write(int block, uint8_t* data) {
  if (block > BLOCK_COUNT) {
    return 0;
  }


  return BLOCK_SIZE;
}

//Consider moving this into a common location where I can ask for just a single block
void Token::readFlash(int page, int len, uint8_t* buffer) {
  dflash->Page_To_Buffer(PRIMARY_BUFFER, page);
  dflash->Buffer_Read_Str(PRIMARY_BUFFER, page, len, (unsigned char*)buffer);
}


void Token::writeFlash(int page, int len, uint8_t* buffer) {
  dflash->Buffer_Write_Str(PRIMARY_BUFFER, page, len, (unsigned char*)buffer);
  dflash->Buffer_To_Page(PRIMARY_BUFFER, page);
}


void Token::import(int libraryId, char *name, uint8_t elementAndType, uint8_t* data) {


  //I need a static version of writeFlash

  uint8_t pageBuffer[PAGE_SIZE];
  //4 pages per token, find our starting point
  int startingPage = libraryId * 4;

  //Page 1, 1/4 of data + 1/2 of name
  memcpy(pageBuffer, data + (PAGE_DATA_SIZE * 0), PAGE_DATA_SIZE);
  memcpy(pageBuffer + PAGE_DATA_SIZE, name, 8);
  //writeFlash(startingPage + 0, PAGE_SIZE, pageBuffer);

  //Page 2, 1/4 of data + 1/2 of name
  memcpy(pageBuffer, data + (PAGE_DATA_SIZE * 1), PAGE_DATA_SIZE);
  memcpy(pageBuffer + PAGE_DATA_SIZE, name + 8, 8);
  //writeFlash(startingPage + 1, PAGE_SIZE, pageBuffer);

  //Page 3, 1/4 of data + 1 byte elementandtype
  memcpy(pageBuffer, data + (PAGE_DATA_SIZE * 2), PAGE_DATA_SIZE);
  memcpy(pageBuffer + PAGE_DATA_SIZE, &elementAndType, sizeof(elementAndType));
  //writeFlash(startingPage + 2, PAGE_DATA_SIZE + sizeof(elementAndType), pageBuffer);

  //Page 4, you get the idea.
  memcpy(pageBuffer, data + (PAGE_DATA_SIZE * 3), PAGE_DATA_SIZE);
  //writeFlash(startingPage + 3, PAGE_DATA_SIZE, pageBuffer);

}
