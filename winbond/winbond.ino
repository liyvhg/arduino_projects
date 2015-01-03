//SPI flash programmer for Winbond W25Q80BV DIP8
//Strap wire length 15cm
// CD4050 used as level shifter 5V to 3.3V
//Pins:
//Flash    CD4050    Arduino Uno
//  8(VCC)   1(VDD)    3.3V
//  1(/CS)   2(OUTA)        
//           3(INA)    10(SS)
//  5(DI)    4(OUTB)        
//           5(INB)    11(MOSI)
//  6(CLK)   6(OUTC)        
//           7(INB)    13(SCK)
//  4        8(VSS)    GND
//  2(DO)              12(MISO)
//  3(/WP)             3.3V
//  7(/HOLD)           3.3V
// CD4050 datasheet: http://www.fairchildsemi.com/ds/CD/CD4049UBC.pdf
// W25Q80BV datasheet: http://www.winbond.com/NR/rdonlyres/4D2BF674-7427-4FC8-AEF0-1A534DF74F16/0/W25Q80BV.pdf


#include <SPI.h>

//Pin Definition
//#define MOSI    11
//#define MISO    12 
//#define SCK     13
//#define SS      10

//SPI command
#define ID    0x9F

//Read Manufacturer, Memory type and capacity
void deviceid()
{
  digitalWrite(SS,LOW); //start spi transfert
  SPI.transfer(ID);     //send JEDEC ID command
  byte MANUFACTURERID = SPI.transfer(0xFF);
  byte MEMORYTYPE = SPI.transfer(0xFF);
  byte CAPACITY = SPI.transfer(0xFF);
  digitalWrite(SS,HIGH); //end spi transfert

  Serial.println(MANUFACTURERID,HEX); //should read "EFh"

}


void setup()
{
  Serial.begin(9600);
  
//  digitalWrite(SS,HIGH); //already declared in SPI.cpp
//  pinMode(SS, OUTPUT);   //already declared in SPI.cpp
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV16); //16MHz div by 16=1MHz
  SPI.setDataMode(SPI_MODE0);
  SPI.begin();
  
}

void loop()
{
  deviceid();
  delay(100);
}
