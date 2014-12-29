#ifndef Token_H
#define Token_H

#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#define BLOCK_SIZE 16
#define BLOCK_COUNT 64

class Token
{
  public:
    Token();
    int read(int block, uint8_t* buffer);
    int write(int block, uint8_t* data);

  private:
    uint8_t data[BLOCK_SIZE * 4];

    uint16_t crc_16_ccitt(uint8_t * data_in, uint16_t data_len);
};


#endif
