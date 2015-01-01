
#include "Photocopier.h"


Photocopier::Photocopier() {

}


void Photocopier::init() {
  dflash.init();
}

void Photocopier::readBlock(int chapter, int block, uint8_t* buffer){
  if (chapter == TOC_CHAPTER) {
    //block is now the libraryid of the token
    int page_offset = block / BLOCKS_PER_PAGE;
    int block_offset = block % PAGE_SIZE;
    bufferPage(TOC_CHAPTER + page_offset);
    memcpy(buffer, pageBuffer + block_offset, BLOCK_SIZE);
  } else {
    int page_offset = block / BLOCKS_PER_PAGE;
    int page = (chapter * CHAPTER_SIZE) + page_offset;
    int block_offset = block % PAGE_SIZE;
    bufferPage(TOC_SIZE + page);
    memcpy(buffer, pageBuffer + block_offset, BLOCK_SIZE);
  }

}

void writeBlock(int chapter, int block, uint8_t* buffer){

}


void Photocopier::bufferPage(int page){
  readFlash(page, PAGE_SIZE, pageBuffer);
}

void Photocopier::readFlash(int page, int len, uint8_t* buffer) {
  dflash.Page_To_Buffer(PRIMARY_BUFFER, page);
  dflash.Buffer_Read_Str(PRIMARY_BUFFER, page, len, (unsigned char*)buffer);
}

void Photocopier::writeFlash(int page, int len, uint8_t* buffer) {
  dflash.Buffer_Write_Str(PRIMARY_BUFFER, page, len, (unsigned char*)buffer);
  dflash.Buffer_To_Page(PRIMARY_BUFFER, page);
}


