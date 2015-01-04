

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

  /*
  Serial.print("readFlash: (chapter, page_offset, block_offset)");
  Serial.print(chapter);
  Serial.print(" ");
  Serial.print(page_offset);
  Serial.print(" ");
  Serial.print(block_offset);
  Serial.println(" ");
  */

  dflash->Page_To_Buffer(chapter + page_offset, PRIMARY_BUFFER);
  //Serial.println("Page loaded into buffer");
  dflash->Buffer_Read_Str(PRIMARY_BUFFER, block_offset, BLOCK_SIZE, buffer);
  //Serial.println("block read into buffer");
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


void Token::import(Dataflash dflash) {
  //'template' of tokens
  uint8_t zeros[BLOCK_SIZE] = {0};
  uint8_t ro[BLOCK_SIZE] = {0, 0, 0, 0, 0, 0, 0x0F, 0x0F, 0x0F, 0x69, 0, 0, 0, 0, 0, 0};
  uint8_t rw[BLOCK_SIZE] = {0, 0, 0, 0, 0, 0, 0x7F, 0x0F, 0x08, 0x69, 0, 0, 0, 0, 0, 0};

  uint8_t buffer[BLOCK_SIZE] = {0};
  int libraryId = 0;

  //get the libraryid
  libraryId = Serial.parseInt();
  Serial.print("library id read:");
  Serial.println(libraryId);
  if (libraryId == 0) return;

  int page = TOC_SIZE + (libraryId * CHAPTER_SIZE);

  //Get 2 blocks of data
  Serial.println("Write block 0 of token");
  Serial.readBytes((char*)buffer, BLOCK_SIZE);

  // First of 4 pages of this token
  dflash.Page_To_Buffer(page, PRIMARY_BUFFER);


  //Print the block you *think* you will write
  for(int i = 0; i < BLOCK_SIZE; i++) {
    Serial.print(buffer[i], HEX);
    Serial.print(" ");
  }
  Serial.println(" ");




  dflash.Buffer_Write_Str(PRIMARY_BUFFER, 0 * BLOCK_SIZE, BLOCK_SIZE, buffer);

  dflash.Buffer_To_Page(PRIMARY_BUFFER, page);

  delay(1000);

  //Print the block you *think* you just wrote
  uint8_t debug[BLOCK_SIZE];
  dflash.Page_To_Buffer(page, PRIMARY_BUFFER);
  dflash.Buffer_Read_Str(PRIMARY_BUFFER, 0 * BLOCK_SIZE, BLOCK_SIZE, debug);
  for(int i = 0; i < BLOCK_SIZE; i++) {
    Serial.print(debug[i], HEX);
    Serial.print(" ");
  }
  Serial.println(" ");

  return;

  Serial.println("Write block 1 of token");
  Serial.readBytes((char*)buffer, BLOCK_SIZE);
  dflash.Buffer_Write_Str(PRIMARY_BUFFER, 1 * BLOCK_SIZE, BLOCK_SIZE, buffer);



  //Fill in remainder of token programatically
  for (int i = 2; i < BLOCK_COUNT; i++) {
    //Transition to next page; save previous, load next.
    if (i % BLOCKS_PER_PAGE == 0) {
      dflash.Buffer_To_Page(PRIMARY_BUFFER, page);
      page++;
      Serial.print("Turn to page "); Serial.println(page);
      dflash.Page_To_Buffer(page, PRIMARY_BUFFER);
    }

    Serial.print(i);
    Serial.print(" ");
    if (i == 3) {
      dflash.Buffer_Write_Str(PRIMARY_BUFFER, i * BLOCK_SIZE, BLOCK_SIZE, ro);
    } else if (i % 4 == 0) {
      dflash.Buffer_Write_Str(PRIMARY_BUFFER, i * BLOCK_SIZE, BLOCK_SIZE, rw);
    } else {
      dflash.Buffer_Write_Str(PRIMARY_BUFFER, i * BLOCK_SIZE, BLOCK_SIZE, zeros);
    }
  }
  dflash.Buffer_To_Page(PRIMARY_BUFFER, page); //Final page save



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



}

