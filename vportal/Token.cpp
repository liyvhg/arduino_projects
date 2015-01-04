

#include "Token.h"

Token::Token(int libraryId, Dataflash* dflash) : libraryId(libraryId), dflash(dflash) {

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

  Serial.print("readFlash: (chapter, page_offset, block_offset)");
  Serial.print(chapter);
  Serial.print(" ");
  Serial.print(page_offset);
  Serial.print(" ");
  Serial.print(block_offset);
  Serial.println(" ");

  dflash->Page_To_Buffer(chapter + page_offset, PRIMARY_BUFFER);
  dflash->Buffer_Read_Str(PRIMARY_BUFFER, block_offset, BLOCK_SIZE, buffer);
}

void Token::writeFlash(int block, uint8_t* buffer) {
  int page = TOC_SIZE + (libraryId * CHAPTER_SIZE);
  int block_offset = (block % PAGE_SIZE) * BLOCK_SIZE;

  dflash->Page_To_Buffer(page, PRIMARY_BUFFER);
  dflash->Buffer_Write_Str(PRIMARY_BUFFER, block_offset, BLOCK_SIZE, buffer);
  dflash->Buffer_To_Page(PRIMARY_BUFFER, page);
}

void Token::display(int libraryId, char* topline, char* bottomline) {
  int page_offset = libraryId / PAGE_SIZE;
  int block_offset = (libraryId % PAGE_SIZE) * BLOCK_SIZE;
  uint8_t buffer[BLOCK_SIZE];

  dflash->Page_To_Buffer(page_offset, PRIMARY_BUFFER);
  dflash->Buffer_Read_Str(PRIMARY_BUFFER, block_offset, BLOCK_SIZE, buffer);

  //Topline Character name
  //Bottomline: Element, type

  Serial.println((char*)buffer);

}


void Token::import(Dataflash *dflash) {
  //'template' of tokens
  uint8_t zeros[BLOCK_SIZE] = {0};
  uint8_t ro[BLOCK_SIZE] = {0, 0, 0, 0, 0, 0, 0x0F, 0x0F, 0x0F, 0x69, 0, 0, 0, 0, 0, 0};
  uint8_t rw[BLOCK_SIZE] = {0, 0, 0, 0, 0, 0, 0x7F, 0x0F, 0x08, 0x69, 0, 0, 0, 0, 0, 0};

  uint8_t buffer[BLOCK_SIZE] = {0};
  int libraryId = 0;

  //get the libraryid
  Serial.println("Import started, send library id");
  libraryId = Serial.parseInt();
  Serial.print("library id read:");
  Serial.println(libraryId);
  if (libraryId == 0) return;

  int page = TOC_SIZE + (libraryId * CHAPTER_SIZE);

  //Get 2 blocks of data
  Serial.println("Write block 0 of token");
  Serial.readBytes((char*)buffer, BLOCK_SIZE);

  // First of 4 pages of this token
  dflash->Page_To_Buffer(page, PRIMARY_BUFFER);

  dflash->Buffer_Write_Str(PRIMARY_BUFFER, 0 * BLOCK_SIZE, BLOCK_SIZE, buffer);

  Serial.println("Write block 1 of token");
  Serial.readBytes((char*)buffer, BLOCK_SIZE);
  dflash->Buffer_Write_Str(PRIMARY_BUFFER, 1 * BLOCK_SIZE, BLOCK_SIZE, buffer);

  //Fill in remainder of token programatically
  for (int i = 2; i < 64; i++) {
    //Transition to next page; save previous, load next.
    if (i % BLOCKS_PER_PAGE == 0) {
      dflash->Buffer_To_Page(PRIMARY_BUFFER, page);
      page++;
      Serial.print("Turn to page "); Serial.println(page);
      dflash->Page_To_Buffer(page, PRIMARY_BUFFER);
    }

    Serial.print("Write block ");
    Serial.println(i);
    if (i == 3) {
      dflash->Buffer_Write_Str(PRIMARY_BUFFER, i * BLOCK_SIZE, BLOCK_SIZE, ro);
    } else if (i % 4 == 0) {
      dflash->Buffer_Write_Str(PRIMARY_BUFFER, i * BLOCK_SIZE, BLOCK_SIZE, rw);
    } else {
      dflash->Buffer_Write_Str(PRIMARY_BUFFER, i * BLOCK_SIZE, BLOCK_SIZE, zeros);
    }

  }

  //Final page save
  dflash->Buffer_To_Page(PRIMARY_BUFFER, page);

  Serial.setTimeout(1000);
}


