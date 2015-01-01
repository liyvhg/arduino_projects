

#include "Token.h"

Token::Token(int libraryId) : libraryId(libraryId) {

}

int Token::display(uint8_t* topline, uint8_t* bottomline) {
  //Topline Character name
  //Bottomline: Element, type

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


