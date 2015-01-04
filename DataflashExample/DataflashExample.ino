
#include <SPI.h>
#include <dataflash.h>

#define DECIMAL 10
#define PRIMARY_BUFFER 1
#define PAGE_START 0
#define PAGE_SIZE 256
#define REQN 6

Dataflash dflash;
const int page = 0;

void setup() {
    Serial.begin(9600);
    while(!Serial);
    Serial.println("HELLO");      // ACK to host

    Serial.print("SS=");Serial.println(SS);
    Serial.print("MOSI=");Serial.println(MOSI);
    Serial.print("MISO=");Serial.println(MISO);
    Serial.print("SCK=");Serial.println(SCK);

    uint8_t status = dflash.init();
    Serial.print("Dataflash status: "); Serial.println(status, BIN);

    char writeBuffer[PAGE_SIZE] = {0};
    char pangram[] = "The quick brown fox jumps over the lazy dog ";
    char clock[10] = {0};
    itoa(millis(), clock, DECIMAL);
    strncat((char*)writeBuffer, pangram, PAGE_SIZE);
    strncat((char*)writeBuffer, clock, 10);

    Serial.print("WRITING: ");
    printString(writeBuffer);

    dflash.Page_To_Buffer(page, PRIMARY_BUFFER);
    dflash.Buffer_Write_Str(PRIMARY_BUFFER, PAGE_START, PAGE_SIZE, (uint8_t*)writeBuffer);
    dflash.Buffer_To_Page(PRIMARY_BUFFER, page);

    delay(1000);

    char readBuffer[PAGE_SIZE] = {0};
    Serial.print("READING: ");

    dflash.Page_To_Buffer(page, PRIMARY_BUFFER);
    dflash.Buffer_Read_Str(PRIMARY_BUFFER, PAGE_START, PAGE_SIZE, (uint8_t*)readBuffer);
    printString(readBuffer);

}


void loop() {
}

void printString(char* buffer) {
    Serial.print(strlen(buffer));
    Serial.print(" ");
    for(int i = 0; i < strlen(buffer); i++) {
      Serial.print((char)buffer[i]);
      Serial.print(" ");
    }
    Serial.println(" ");
    //Serial.println(buffer);
}
