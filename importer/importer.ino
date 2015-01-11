
// Import libraries (BLEPeripheral depends on SPI)
#include <SPI.h>
#include <dataflash.h>

//serLCD
#define LCD Serial1
#define BACKLIGHT_CMD 0x7C
#define BACKLIGHT_BASE 0x80
#define BACKLIGHT_LEVELS 30

#define LCD_MOVE 0xFE
#define LCD_TOP 0x80
#define LCD_BOTTOM 0xC0
#define LCD_CLEAR 0x01

#define PRIMARY_BUFFER 1
#define SECONDARY_BUFFER 2

#define BLOCK_SIZE 16
#define BLOCK_COUNT 64

#define PAGE_SIZE 256 //bytes
#define PAGE_COUNT 2048 //4Mbit chip

#define BLOCKS_PER_PAGE 16

#define TOC_CHAPTER 0
#define TOC_SIZE 16 //Pages
#define CHAPTER_SIZE 4 //pages in a chapter/token

void setup() {
    LCD.begin(9600);
    LCD.write(LCD_MOVE);
    LCD.write(LCD_CLEAR);
    LCD.write(BACKLIGHT_CMD);
    LCD.write(BACKLIGHT_BASE + BACKLIGHT_LEVELS - 1);

    Serial.begin(115200);
    delay(3000);  //3 seconds delay for enabling to see the start up comments on the serial board

    LCD.print("Importer");
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

  //get the libraryid
  libraryId = Serial.parseInt();
  if (libraryId < 0) return;

  // First of 4 pages of this token
  int page = TOC_SIZE + (libraryId * CHAPTER_SIZE);
  dflash.Page_To_Buffer(page, PRIMARY_BUFFER);

  //Get 2 blocks of data
  Serial.readBytes((char*)buffer, BLOCK_SIZE);
  dflash.Buffer_Write_Str(PRIMARY_BUFFER, 0 * BLOCK_SIZE, BLOCK_SIZE, buffer);
  LCD.print("*");

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

  LCD.print(" ");
}


void importNames() {
  uint8_t buffer[BLOCK_SIZE] = {0};
  int count = 0;
  Dataflash dflash;
  uint8_t status = dflash.init();

  //get the count
  count = Serial.parseInt();
  if (count < 1) return;

  // First of 16 pages of Table of contents
  int page = 0;
  dflash.Page_To_Buffer(page, PRIMARY_BUFFER);

  for (int i = 0; i < count; i++) {
    int offset = (i % BLOCKS_PER_PAGE) * BLOCK_SIZE;
    memset(buffer, 0, BLOCK_SIZE);

    //Transition to next page; save previous, load next.
    if (i % BLOCKS_PER_PAGE == 0 && i > 0) {
      dflash.Buffer_To_Page(PRIMARY_BUFFER, page);
      page++;
      if (page > TOC_SIZE) {
        Serial.print("[ERROR] writing to write to page outside TOC");
        return;
      }
      dflash.Page_To_Buffer(page, PRIMARY_BUFFER);
    }

    Serial.readBytes((char*)buffer, BLOCK_SIZE);
    LCD.write(LCD_MOVE);
    LCD.write(LCD_CLEAR);

    LCD.write(LCD_MOVE);
    LCD.write(LCD_TOP);
    LCD.print(i);
    LCD.print("/");
    LCD.print(count);
    LCD.print(" (");
    LCD.print(page);
    LCD.print(")");

    LCD.write(LCD_MOVE);
    LCD.write(LCD_BOTTOM);
    LCD.write((char*)buffer, strlen((char*)buffer));

    dflash.Buffer_Write_Str(PRIMARY_BUFFER, offset, BLOCK_SIZE, buffer);
  }
  dflash.Buffer_To_Page(PRIMARY_BUFFER, page); //Final page save

}
