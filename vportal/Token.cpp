

#include "Token.h"

Token::Token(int libraryId) : libraryId(libraryId), dflash() {
    uint8_t status = dflash.init();
    Serial.print(F("Dataflash status: ")); Serial.println(status, BIN);
}

int Token::read(int block, uint8_t* buffer) {
  if (block > BLOCK_COUNT) {
    return 0;
  }

  readFlash(block, buffer);

  return BLOCK_SIZE;
}

int Token::write(int block, uint8_t* buffer) {
  if (block > BLOCK_COUNT) {
    return 0;
  }

  writeFlash(block, buffer);

  return BLOCK_SIZE;
}

void Token::readFlash(int block, uint8_t* buffer) {
  int chapter = TOC_SIZE + (libraryId * CHAPTER_SIZE);
  int page_offset = block / PAGE_SIZE; //Which page in chapter [0,3]
  int block_offset = (block % PAGE_SIZE) * BLOCK_SIZE;

/*
  Serial.print(F("readFlash: (chapter, page_offset, block_offset)"));
  Serial.print(chapter);
  Serial.print(F(" "));
  Serial.print(page_offset);
  Serial.print(F(" "));
  Serial.print(block_offset);
  Serial.println(F(" "));
*/

  dflash.Page_To_Buffer(chapter + page_offset, PRIMARY_BUFFER);
  //Serial.println(F("Page loaded into buffer"));
  dflash.Buffer_Read_Str(PRIMARY_BUFFER, block_offset, BLOCK_SIZE, buffer);
  //Serial.println(F("block read into buffer"));
}

void Token::writeFlash(int block, uint8_t* buffer) {
  int page = TOC_SIZE + (libraryId * CHAPTER_SIZE);
  int block_offset = (block % PAGE_SIZE) * BLOCK_SIZE;

  dflash.Page_To_Buffer(page, PRIMARY_BUFFER);
  dflash.Buffer_Write_Str(PRIMARY_BUFFER, block_offset, BLOCK_SIZE, buffer);
  dflash.Buffer_To_Page(PRIMARY_BUFFER, page);
}

void Token::display(int libraryId, char* topline, char* bottomline) {
  int page_offset = libraryId / PAGE_SIZE;
  int block_offset = (libraryId % PAGE_SIZE) * BLOCK_SIZE;
  uint8_t buffer[BLOCK_SIZE];

  dflash.Page_To_Buffer(page_offset, PRIMARY_BUFFER);
  dflash.Buffer_Read_Str(PRIMARY_BUFFER, block_offset, BLOCK_SIZE, buffer);

  //Topline Character name
  //Bottomline: Element, type

  Serial.println((char*)buffer);

}


void Token::import() {
  Dataflash dflash;
  uint8_t status = dflash.init();

  //'template' of tokens
  uint8_t zeros[BLOCK_SIZE] = {0};
  uint8_t ro[BLOCK_SIZE] = {0, 0, 0, 0, 0, 0, 0x0F, 0x0F, 0x0F, 0x69, 0, 0, 0, 0, 0, 0};
  uint8_t rw[BLOCK_SIZE] = {0, 0, 0, 0, 0, 0, 0x7F, 0x0F, 0x08, 0x69, 0, 0, 0, 0, 0, 0};

  uint8_t buffer[BLOCK_SIZE] = {0};
  int libraryId = 0;

  //get the libraryid
  libraryId = Serial.parseInt();
  Serial.print(F("library id read:"));
  Serial.println(libraryId);
  if (libraryId == 0) return;

  // First of 4 pages of this token
  int page = TOC_SIZE + (libraryId * CHAPTER_SIZE);
  dflash.Page_To_Buffer(page, PRIMARY_BUFFER);

  //Get 2 blocks of data
  Serial.println(F("Write block 0 of token"));
  Serial.readBytes((char*)buffer, BLOCK_SIZE);
  dflash.Buffer_Write_Str(PRIMARY_BUFFER, 0 * BLOCK_SIZE, BLOCK_SIZE, buffer);

  Serial.println(F("Write block 1 of token"));
  Serial.readBytes((char*)buffer, BLOCK_SIZE);
  dflash.Buffer_Write_Str(PRIMARY_BUFFER, 1 * BLOCK_SIZE, BLOCK_SIZE, buffer);

  //Fill in remainder of token programatically
  for (int i = 2; i < BLOCK_COUNT; i++) {
    //Transition to next page; save previous, load next.
    if (i % BLOCKS_PER_PAGE == 0) {
      dflash.Buffer_To_Page(PRIMARY_BUFFER, page);
      page++;
      Serial.print(F("|"));
      dflash.Page_To_Buffer(page, PRIMARY_BUFFER);
    }

    Serial.print(F("."));
    if (i == 3) {
      dflash.Buffer_Write_Str(PRIMARY_BUFFER, i * BLOCK_SIZE, BLOCK_SIZE, ro);
    } else if (i % 4 == 0) {
      dflash.Buffer_Write_Str(PRIMARY_BUFFER, i * BLOCK_SIZE, BLOCK_SIZE, rw);
    } else {
      dflash.Buffer_Write_Str(PRIMARY_BUFFER, i * BLOCK_SIZE, BLOCK_SIZE, zeros);
    }
  }
  dflash.Buffer_To_Page(PRIMARY_BUFFER, page); //Final page save

  Serial.println(F(" "));

/*
  //dump the saved token's first page
  page = TOC_SIZE + (libraryId * CHAPTER_SIZE);
  Serial.print("Page dump "); Serial.println(page);
  for(int i = 0; i < BLOCKS_PER_PAGE; i++) {
    dflash.Page_To_Buffer(page, PRIMARY_BUFFER);
    uint8_t buffer[BLOCK_SIZE] = {0};
    dflash.Buffer_Read_Str(PRIMARY_BUFFER, i * BLOCK_SIZE, BLOCK_SIZE, buffer);
    for(int j = 0; j < BLOCK_SIZE; j++) {
      Serial.print(buffer[j], HEX);
    }
    Serial.println(" ");
  }
*/

}

