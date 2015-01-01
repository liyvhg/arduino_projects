

#include "Token.h"

void Token::display(int libraryId, char* topline, char* bottomline) {
  //Topline Character name
  //Bottomline: Element, type

}

Token::Token(int libraryId) : libraryId(libraryId) {

}

int Token::read(int block, uint8_t* buffer) {
  if (block > BLOCK_COUNT) {
    return 0;
  }

  //dflash.readBlock(libraryId, block, buffer);

  return BLOCK_SIZE;
}

int Token::write(int block, uint8_t* data) {
  if (block > BLOCK_COUNT) {
    return 0;
  }


  return BLOCK_SIZE;
}


