
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
  int page_offset = block / BLOCKS_PER_PAGE; //Which page in chapter [0,3]
  int block_offset = (block % BLOCKS_PER_PAGE) * BLOCK_SIZE;

  dflash.Page_Read_Str(chapter + page_offset, block_offset, BLOCK_SIZE, buffer);
}

void Token::writeFlash(int block, uint8_t* buffer) {
  int chapter = TOC_SIZE + (libraryId * CHAPTER_SIZE);
  int page_offset = block / BLOCKS_PER_PAGE; //Which page in chapter [0,3]
  int block_offset = (block % BLOCKS_PER_PAGE) * BLOCK_SIZE;

  uint8_t dflash_buffer;

  switch(elementAndType & TYPE_MASK) {
    case TRAP_MASTER:
    case MINI:
    case REGULAR:
      dflash_buffer = PRIMARY_BUFFER;
      break;
    case TRAP:
      dflash_buffer = SECONDARY_BUFFER;
      break;
  }

  dflash.Page_To_Buffer(chapter + page_offset, dflash_buffer);
  dflash.Buffer_Write_Str(dflash_buffer, block_offset, BLOCK_SIZE, buffer);
  dflash.Buffer_To_Page(dflash_buffer, chapter + page_offset);
}

void Token::display() {
  int page_offset = libraryId / BLOCKS_PER_PAGE;
  int block_offset = (libraryId % BLOCKS_PER_PAGE) * BLOCK_SIZE;

  dflash.Page_Read_Str(page_offset, block_offset, BLOCK_SIZE-1, (uint8_t*)name);
  dflash.Page_Read_Str(page_offset, block_offset, 1, &elementAndType);

  //Topline Character name
  LCD.write(LCD_MOVE);
  LCD.write(LCD_TOP);
  LCD.print(name);

  //Bottomline: Element, type
  LCD.write(LCD_MOVE);
  LCD.write(LCD_BOTTOM);

  switch(element()) {
    case MAGIC:
    case EARTH:
    case WATER:
    case FIRE:
    case TECH:
    case UNDEAD:
    case LIFE:
    case AIR:
    case DARK:
    case LIGHT:
      LCD.print("Element");
      break;
  }

  LCD.write(LCD_MOVE);
  LCD.write(LCD_BOTTOM + 8); //2nd line, half way across

  switch(type()) {
    case TRAP_MASTER:
    case TRAP:
    case MAGIC_ITEM:
    case LOCATION:
    case MINI:
    case REGULAR:
      LCD.print("Type");
      break;
  }

}

uint8_t Token::type() {
  return (elementAndType & TYPE_MASK);
}

uint8_t Token::element() {
  return (elementAndType & ELEMENT_MASK);
}


#ifdef TOKEN_IMPORT
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
  if (libraryId == 0) return;

  // First of 4 pages of this token
  int page = TOC_SIZE + (libraryId * CHAPTER_SIZE);
  dflash.Page_To_Buffer(page, PRIMARY_BUFFER);

  //Get 2 blocks of data
  Serial.readBytes((char*)buffer, BLOCK_SIZE);
  dflash.Buffer_Write_Str(PRIMARY_BUFFER, 0 * BLOCK_SIZE, BLOCK_SIZE, buffer);
  Serial.print(F("*"));

  Serial.readBytes((char*)buffer, BLOCK_SIZE);
  dflash.Buffer_Write_Str(PRIMARY_BUFFER, 1 * BLOCK_SIZE, BLOCK_SIZE, buffer);
  Serial.print(F("*"));

  //Fill in remainder of token programatically
  for (int i = 2; i < BLOCK_COUNT; i++) {
    //Transition to next page; save previous, load next.
    if (i % BLOCKS_PER_PAGE == 0) {
      Serial.println(" ");
      dflash.Buffer_To_Page(PRIMARY_BUFFER, page);
      page++;
      dflash.Page_To_Buffer(page, PRIMARY_BUFFER);
    }

    int offset = (i % BLOCKS_PER_PAGE) * BLOCK_SIZE;
    if (i == 3) {
      Serial.print(F("r"));
      dflash.Buffer_Write_Str(PRIMARY_BUFFER, offset, BLOCK_SIZE, ro);
    } else if (i % 4 == 3) {
      Serial.print(F("w"));
      dflash.Buffer_Write_Str(PRIMARY_BUFFER, offset, BLOCK_SIZE, rw);
    } else {
      Serial.print(F("0"));
      dflash.Buffer_Write_Str(PRIMARY_BUFFER, offset, BLOCK_SIZE, zeros);
    }
  }
  dflash.Buffer_To_Page(PRIMARY_BUFFER, page); //Final page save

  Serial.println(F(" "));

}
#endif
