

#include "Token.h"


int Token::read(int block, uint8_t* buffer) {
  memcpy(buffer, data+(block*BLOCK_SIZE), BLOCK_SIZE);
}

int Token::write(int block, uint8_t* data) {

}


uint16_t Token::crc_16_ccitt(uint8_t * data_in, uint16_t data_len) {
  uint16_t i;
  uint16_t crc = 0xffff;

  for(i = 0; i < data_len; i++) {
    crc  = (unsigned char)(crc >> 8) | (crc << 8);
    crc ^= data_in[i];
    crc ^= (unsigned char)(crc & 0xff) >> 4;
    crc ^= (crc << 8) << 4;
    crc ^= ((crc & 0xff) << 4) << 1;
  }

  return crc;
}

Token::Token() {
  Serial.println("Token: About to initialize temp");
  uint8_t temp[BLOCK_SIZE * 4] = {
    0xaf, 0xbe, 0xe9, 0xef, 0x17, 0x81, 0x01, 0x0f, 0xc4, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x14,
    0xe4, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x30, 0x31, 0xa2,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0x0f, 0x0f, 0x0f, 0x69, 0, 0, 0, 0, 0, 0
  };

  Serial.println("Token: About to memcpy token data");
  memcpy(data, temp, BLOCK_SIZE * 4);
}


