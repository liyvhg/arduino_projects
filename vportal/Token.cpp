

#include "Token.h"

Token::Token(Dataflash *dflash, int libraryId) : dflash(dflash) {
  //Read in character name and save
  //The pages are 256 + 8 bytes, so I store the character name at the end of the first 2 pages
  readFlash(0, PAGE_SIZE, pageBuffer);
  memcpy(name, pageBuffer + 256, 8);
}


int Token::read(int block, uint8_t* buffer) {
  if (block > BLOCK_COUNT) {
    return 0;
  }

  int page = block / PAGES_PER_TOKEN;
  int offset = (block % PAGES_PER_TOKEN) * BLOCK_SIZE;

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

