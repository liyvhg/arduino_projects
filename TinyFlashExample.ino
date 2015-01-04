// Winbond serial flash loader.  Runs on normal Arduino (e.g. Uno,
// NOT Trinket), works with 'AudioXfer' Processing code on host PC.

#include <Adafruit_TinyFlash.h>
#include <SPI.h>

#define LED A9
#define PAGE_SIZE 250
#define DECIMAL 10

// Blink LED (not pin 13) to indicate flash error
void error(uint32_t d) {
  for(;;) {
    digitalWrite(LED, HIGH);
    delay(d);
    digitalWrite(LED, LOW);
    delay(d);
  }
}

int page = 1;
Adafruit_TinyFlash flash;

void setup() {
  digitalWrite(SS,HIGH); //already declared in SPI.cpp
  pinMode(SS, OUTPUT);   //already declared in SPI.cpp

  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);

  Serial.begin(9600);
  while(!Serial);
  Serial.println("HELLO");      // ACK to host

  Serial.print("SS=");Serial.println(SS);
  Serial.print("MOSI=");Serial.println(MOSI);
  Serial.print("MISO=");Serial.println(MISO);
  Serial.print("SCK=");Serial.println(SCK);

  uint32_t capacity = 0;
  do  {
    delay(500); //seems like it need a second sometimes
    capacity = flash.begin();
    Serial.print("Capacity: "); Serial.println(capacity);     // Chip size to host
  } while(!capacity);

  Serial.println("ERASING SECTOR");
  if(flash.eraseSector(page)) {
    Serial.println("Successful");
  } else {
    Serial.println("Erasing chip failed");
  }

}

void loop() {

  char writeBuffer[PAGE_SIZE] = {0};
  char pangram[] = "The quick brown fox jumps over the lazy dog ";
  char clock[10] = {0};
  itoa(millis(), clock, DECIMAL);
  strncat((char*)writeBuffer, pangram, PAGE_SIZE);
  strncat((char*)writeBuffer, clock, 10);

  Serial.print("WRITING: ");

  Serial.print(strlen(writeBuffer));
  Serial.print(" ");
  Serial.println(writeBuffer);
  if (flash.writePage(page, (uint8_t*)writeBuffer)) {
    Serial.println("Success");
  } else {
    Serial.println("Error writing");
  }

  delay(500);


  char readBuffer[PAGE_SIZE] = {0};
  Serial.print("READING: ");

  flash.beginRead(page);
  for(int i = 0; i < PAGE_SIZE; i++) {
    readBuffer[i] = flash.readNextByte();
  }
  flash.endRead();
  Serial.print(strlen(readBuffer));
  Serial.print(" ");
  Serial.println(readBuffer);

  delay(2000);

}

