
#include "VirtualPortal.h"


VirtualPortal::VirtualPortal() : lightVal(0), sequence(0), reset_6(0x00), reset_8(0xc2) {
    characterToken = NULL;
}

int VirtualPortal::respondTo(uint8_t* message, uint8_t* response) {

  printCommand(true, message);

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
      write(message, response);
      break;
  }

  printCommand(false, response);

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

    response[0] = 'Q';
    response[1] = index;
    response[2] = block;

    bool debug = false;
    if (debug) {
      switch(block) {
        case 0:
          response[3] = 0xAF;
          response[4] = 0xBE;
          response[5] = 0xE9;
          response[6] = 0xEF;
          response[7] = 0x17;
          response[8] = 0x81;
          response[9] = 0x01;
          response[10] = 0x0F;
          response[11] = 0xC4;
          response[12] = 0x07;
          response[18] = 0x14;
          break;
        case 1:
          response[3] = 0xE4;
          response[4] = 0x01;
          response[15] = 0x04;
          response[16] = 0x30;
          response[17] = 0x31;
          response[18] = 0xA2;
          break;
        default:
          break;
      }
    } else {
      characterToken->read(block, response+3);
    }

    return BLE_ATTRIBUTE_MAX_VALUE_LENGTH;
}

int VirtualPortal::write(uint8_t* message, uint8_t* response) {
    int index = message[1];
    int block = message[2];
    int arrayIndex = index & 0x0f;

    characterToken->write(block, response+3);

    //Status update with different first 3 bytes
    status(response);
    response[0] = 'W';
    response[1] = index;
    response[2] = block;

    return BLE_ATTRIBUTE_MAX_VALUE_LENGTH;
}

int VirtualPortal::reset(uint8_t* response) {

  //52021900 000100aa c2021900
  //52021900 000201aa c3021900
  //52021900 000301aa c3021900

  response[0] = 'R';
  response[1] = 0x02;
  response[2] = 0x19;
  response[3] = 0;
  response[4] = 0;
  response[5] = sequence++ % 0xFF;
  response[6] = reset_6;
  response[7] = 0xaa;
  response[8] = reset_8;
  response[9] = 0x02;
  response[10] = 0x19;


  //If we come into this method again, use different values
  if (reset_6 == 0) reset_6++;
  if (reset_8 == 0xc2) reset_8++;

}

int VirtualPortal::status(uint8_t* response) {
  uint8_t temp[BLE_ATTRIBUTE_MAX_VALUE_LENGTH] = {'S', 0, 0, 0, 0, 0xFF, 1, 0xaa, 0xc3, 2, 0x19, 0, 0, 0, 0, 0, 0, 0, 0, 0};
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

void VirtualPortal::printCommand(bool incoming, const uint8_t* command) {
  int interestingBytes = 0;

  LCD.write(0xFE); //MoveTo
  LCD.write(0xCF); //2nd line, Last character
  LCD.write(command[0]);

  switch(command[0]) {
    case 'C':
      interestingBytes = 3;
      break;
    case 'L': //Trap light
      interestingBytes = 1;
      break;
    case 'A':
    case 'Q': //Query / read
    case 'S':
    case 'R':
    case 'W': //Write
      interestingBytes = BLE_ATTRIBUTE_MAX_VALUE_LENGTH - 1;
      break;
  }

  if (interestingBytes > 0) {
    Serial.print(incoming ? "<= " : "=> ");
    Serial.print((char)command[0]);
    Serial.print(F(" "));

    for(int i = 0; i < interestingBytes; i++) {
      Serial.print(command[i+1], HEX); //+1 to ignore ASCII first byte
      Serial.print(F(" "));
    }
    if (incoming) {
      Serial.print(F(" "));
    } else {
      Serial.println(F(" "));
    }
  }
}

/*
void VirtualPortal::printHex(String prefix, const unsigned char* buffer, int len, String suffix) {
    Serial.print(prefix);
    for(int i = 0; i < len; i++) {
      Serial.print(buffer[i], HEX);
      Serial.print(" ");
    }
    Serial.println(suffix);
}
*/


void VirtualPortal::connect() {

}

void VirtualPortal::disconnect() {

}

void VirtualPortal::subscribe() {
  sequence = 0;
}

void VirtualPortal::unsubscribe() {

}
