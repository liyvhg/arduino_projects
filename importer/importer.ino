
// Import libraries (BLEPeripheral depends on SPI)
#include <SPI.h>
#include <dataflash.h>
#include "Token.h"

#define LCD Serial
#define LCD_MOVE "\n"

void setup() {
  /*
    LCD.begin(9600);
    LCD.write(LCD_MOVE);
    LCD.write(LCD_CLEAR);
    LCD.write(BACKLIGHT_CMD);
    LCD.write(BACKLIGHT_BASE + BACKLIGHT_LEVELS - 1);
*/
    Serial.begin(115200);
    LCD.print("Importer ready");

    delay(3000);
    Serial.print("TOC_SIZE=");Serial.println(TOC_SIZE);
}

void loop() {
  if (Serial.available() > 0) {
    int incomingByte = Serial.read();
    if (incomingByte == 'I') { //import tokens
      import();
    }
    if (incomingByte == 'N') { //import names
      importNames();
    }
    if (incomingByte == 'L') {
      listNames();
    }
    if (incomingByte == 'D') {
      dumpToken();
    }
  }

}

void import() {
  Dataflash dflash;
  uint8_t status = dflash.init();

  //'template' of tokens
  uint8_t zeros[BLOCK_SIZE] = {0};
  uint8_t ro[BLOCK_SIZE] = {0, 0, 0, 0, 0, 0, 0x0F, 0x0F, 0x0F, 0x69, 0, 0, 0, 0, 0, 0};
  uint8_t rw[BLOCK_SIZE] = {0, 0, 0, 0, 0, 0, 0x7F, 0x0F, 0x08, 0x69, 0, 0, 0, 0, 0, 0};

  uint8_t buffer[BLOCK_SIZE] = {0};
  int libraryId = 0;

  while (Serial.available() < 1);
  //get the libraryid
  libraryId = Serial.parseInt();
  if (libraryId < 0) return;

  // First of 4 pages of this token
  int page = TOC_SIZE + (libraryId * CHAPTER_SIZE);
  dflash.Page_To_Buffer(page, PRIMARY_BUFFER);

  while (Serial.available() < 1);

  //Get 2 blocks of data
  Serial.readBytes((char*)buffer, BLOCK_SIZE);
  dflash.Buffer_Write_Str(PRIMARY_BUFFER, 0 * BLOCK_SIZE, BLOCK_SIZE, buffer);
  LCD.print("*");

  while (Serial.available() < 1);

  Serial.readBytes((char*)buffer, BLOCK_SIZE);
  dflash.Buffer_Write_Str(PRIMARY_BUFFER, 1 * BLOCK_SIZE, BLOCK_SIZE, buffer);
  LCD.print("*");

  //Fill in remainder of token programatically
  for (int i = 2; i < BLOCK_COUNT; i++) {
    //Transition to next page; save previous, load next.
    if (i % BLOCKS_PER_PAGE == 0) {
      LCD.print("|");
      dflash.Buffer_To_Page(PRIMARY_BUFFER, page);
      page++;
      dflash.Page_To_Buffer(page, PRIMARY_BUFFER);
    }

    int offset = (i % BLOCKS_PER_PAGE) * BLOCK_SIZE;
    if (i == 3) {
      LCD.print("r");
      dflash.Buffer_Write_Str(PRIMARY_BUFFER, offset, BLOCK_SIZE, ro);
    } else if (i % 4 == 3) {
      LCD.print("w");
      dflash.Buffer_Write_Str(PRIMARY_BUFFER, offset, BLOCK_SIZE, rw);
    } else {
      LCD.print("0");
      dflash.Buffer_Write_Str(PRIMARY_BUFFER, offset, BLOCK_SIZE, zeros);
    }
  }
  dflash.Buffer_To_Page(PRIMARY_BUFFER, page); //Final page save

}


void importNames() {
  uint8_t buffer[BLOCK_SIZE] = {0};
  int count = 0;
  Dataflash dflash;
  uint8_t status = dflash.init();

  //get the count
  count = Serial.parseInt();
  if (count < 1) return;

  // First of page of Table of contents
  int page = 0;
  dflash.Page_To_Buffer(page, SECONDARY_BUFFER);

  for (int i = 0; i < count; i++) {
    int offset = (i % BLOCKS_PER_PAGE) * BLOCK_SIZE;
    memset(buffer, 0, BLOCK_SIZE);
    LCD.write(LCD_MOVE);
    LCD.write(LCD_CLEAR);

    LCD.write(LCD_MOVE);
    LCD.write(LCD_TOP);
    LCD.print(i+1);
    LCD.print("/");
    LCD.print(count);

    //Transition to next page; save previous, load next.
    if (i % BLOCKS_PER_PAGE == 0 && i > 0) {
      dflash.Buffer_To_Page(SECONDARY_BUFFER, page);
      page++;
      if (page > TOC_SIZE) {
        return;
      }
      dflash.Page_To_Buffer(page, SECONDARY_BUFFER);
    }
    LCD.print(" (");
    LCD.print(page);
    LCD.print(")");

    while (Serial.available() < 1);
    int len = Serial.readBytes((char*)buffer, BLOCK_SIZE);
    dflash.Buffer_Write_Str(SECONDARY_BUFFER, offset, BLOCK_SIZE, buffer);

    LCD.write(LCD_MOVE);
    LCD.write(LCD_BOTTOM);
    LCD.write((char*)buffer, min(strlen((char*)buffer), BLOCK_SIZE-1));
  }
  dflash.Buffer_To_Page(SECONDARY_BUFFER, page); //Final page save

}

void dumpToken() {
  Dataflash dflash;
  uint8_t status = dflash.init();

  uint8_t buffer[BLOCK_SIZE] = {0};
  char printBuffer[BLOCK_SIZE * 3] = {0};
  int libraryId = 0;

  while (Serial.available() < 1);
  //get the libraryid
  libraryId = Serial.parseInt();
  if (libraryId < 0) return;
  int chapter = TOC_SIZE + (libraryId * CHAPTER_SIZE);

  Serial.println("--------------------------------");
  Serial.print("library Id: "); Serial.print(libraryId);
  Serial.print(" -> chapter (starting page) "); Serial.println(chapter);

  for (int i = 0; i < 4; i++) {
    int page_offset = i / BLOCKS_PER_PAGE; //Which page in chapter [0,3]
    int block_offset = (i % BLOCKS_PER_PAGE) * BLOCK_SIZE;
    dflash.Page_Read_Str(chapter + page_offset, block_offset, BLOCK_SIZE, buffer);
    sprintf(printBuffer, "%02i: %02X%02X%02X%02X%02X%02X%02X%02X %02X%02X%02X%02X%02X%02X%02X%02X", i,
      buffer[0],
      buffer[1],
      buffer[2],
      buffer[3],
      buffer[4],
      buffer[5],
      buffer[6],
      buffer[7],
      buffer[8],
      buffer[9],
      buffer[10],
      buffer[11],
      buffer[12],
      buffer[13],
      buffer[14],
      buffer[15]
    );


    Serial.println(printBuffer);
  }

}

void listNames() {
  Dataflash dflash;
  uint8_t status = dflash.init();

  char name[BLOCK_SIZE] = {0};
  char printBuffer[BLOCK_SIZE * 3] = {0};
  uint8_t name_len = BLOCK_SIZE - 1;

  Serial.println("--------------------------------");
  Serial.println("Table of contents");
  for (int i = 0; i < 200; i++) {
    int page_offset = i / BLOCKS_PER_PAGE;
    int block_offset = (i % BLOCKS_PER_PAGE) * BLOCK_SIZE;
    dflash.Page_Read_Str(page_offset, block_offset, name_len, (uint8_t*)name);
    sprintf(printBuffer, "%02i: %02X%02X%02X%02X%02X%02X%02X%02X %02X%02X%02X%02X%02X%02X%02X%02X ", i,
      name[0],
      name[1],
      name[2],
      name[3],
      name[4],
      name[5],
      name[6],
      name[7],
      name[8],
      name[9],
      name[10],
      name[11],
      name[12],
      name[13],
      name[14],
      name[15]
    );

    Serial.print(printBuffer);
    Serial.print(name);
    Serial.println(" ");
  }

}
