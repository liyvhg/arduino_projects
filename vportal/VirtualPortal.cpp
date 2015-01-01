
#include "VirtualPortal.h"


VirtualPortal::VirtualPortal() : lightVal(0), sequence(0) {
    characterToken = NULL;
}

int VirtualPortal::respondTo(uint8_t* message, uint8_t* response) {

  switch(message[0]) {
    case 'A':
      activate(message, response);
      break;
    case 'C': //Ring color R G B
      return 0; //No response
      break;
    case 'J':
      return 0; //No response
      break;
    case 'L': //Trap light
      light(message);
      return 0; //No response
      break;
    case 'Q': //Query / read
      query(message, response);
      break;
    case 'R':
      reset(response);
      break;
    case 'S': //Status
      status(response);
      break;
    case 'W': //Write
      break;
  }

  return BLE_ATTRIBUTE_MAX_VALUE_LENGTH;
}


int VirtualPortal::activate(uint8_t* message, uint8_t* response) {
  response[0] = message[0];
  response[1] = message[1];
  response[2] = 0x62;
  response[3] = 0x02;
  response[4] = 0x19;
  response[5] = 0xaa;
  response[6] = 0x01;
  response[7] = 0x53;
  response[8] = 0xbc;
  response[9] = 0x58;
  response[10] = 0xfc;
  response[11] = 0x7d;
  response[12] = 0xf4;

}

int VirtualPortal::query(uint8_t* message, uint8_t* response) {
    int index = message[1];
    int block = message[2];
    int arrayIndex = index & 0x0f;

    response[0] = message[0]; //'Q'
    response[1] = arrayIndex;
    response[2] = block;

    //characterToken->read(block, response+3);

    return BLE_ATTRIBUTE_MAX_VALUE_LENGTH;
}

int VirtualPortal::reset(uint8_t* response) {

  response[0] = 0x52;
  response[1] = 0x02;
  response[2] = 0x19;
  response[5] = sequence++ % 0xFF;
  response[7] = 0xaa;
  response[8] = 0x86;
  response[9] = 0x02;
  response[10] = 0x19;

}

int VirtualPortal::status(uint8_t* response) {
  uint8_t temp[BLE_ATTRIBUTE_MAX_VALUE_LENGTH] = {'S', 0, 0, 0, 0, 0xFF, 1, 0xaa, 0x86, 2, 0x19, 0, 0, 0, 0, 0, 0, 0, 0};
  memcpy(response, temp, BLE_ATTRIBUTE_MAX_VALUE_LENGTH);

  response[1] = characterToken ? 0x01 : 0x00;

  response[5] = sequence++ % 0xFF;

  return BLE_ATTRIBUTE_MAX_VALUE_LENGTH;
}

bool VirtualPortal::loadToken(Token *t) {

  //Switch on Token type to set to right var
  //If a token is being replaced, destruct previous
  characterToken = t;
}

int VirtualPortal::light(uint8_t* message) {
  lightVal = message[1];
}

uint8_t VirtualPortal::light() {
  return lightVal;
}
