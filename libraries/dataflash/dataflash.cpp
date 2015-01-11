
#include "Dataflash.h"

#define CHIP_SELECT   chipSelect();
#define CHIP_DESELECT chipDeselect();

//Hard coding for AT45DB041B (sorry!)
uint8_t PageBits = 11;
unsigned int PageSize = 256;

SPISettings dflashSettings(1000000, MSBFIRST, SPI_MODE3);

Dataflash::Dataflash() {
}

Dataflash::~Dataflash() {
  digitalWrite(SS, HIGH);
}

uint8_t Dataflash::init(void) {
    SPI.begin();

    CHIP_SELECT
    uint8_t status = waitForReady(ONE_SECOND);
    CHIP_DESELECT

    return status;
}

/*****************************************************************************
 *
 *	Function name : Page_To_Buffer
 *
 *	Returns :		None
 *
 *	Parameters :	BufferNo	->	Decides usage of either buffer 1 or 2
 *			PageAdr		->	Address of page to be transferred to buffer
 *
 *	Purpose :	Transfers a page from flash to Dataflash SRAM buffer
 *
 ******************************************************************************/
void Dataflash::Page_To_Buffer (unsigned int PageAdr, uint8_t BufferNo) {
    //24 byte addressing rrrrPPPP PPPPPPPx xxxxxxxx
    uint8_t high_byte = PageAdr >> (16 - PageBits);
    uint8_t mid_byte = PageAdr << (PageBits - 8);
    uint8_t low_byte = 0;

    CHIP_SELECT
    if (1 == BufferNo) {			//transfer flash page to buffer 1
        SPI.transfer(FlashToBuf1Transfer);				//transfer to buffer 1 op-code
    } else if (2 == BufferNo) {					//transfer flash page to buffer 2
        SPI.transfer(FlashToBuf2Transfer);				//transfer to buffer 2 op-code
    }

    SPI.transfer(high_byte);
    SPI.transfer(mid_byte);
    SPI.transfer(low_byte);

    strobe();
    waitForReady(ONE_SECOND);
    CHIP_DESELECT
}

/*****************************************************************************
 *
 *	Function name : Buffer_Read_Str
 *
 *	Returns :		None
 *
 *	Parameters :	BufferNo	->	Decides usage of either buffer 1 or 2
 *					IntPageAdr	->	Internal page address
 *					No_of_bytes	->	Number of bytes to be read
 *					*BufferPtr	->	address of buffer to be used for read bytes
 *
 *	Purpose :		Reads one or more bytes from one of the Dataflash
 *					internal SRAM buffers, and puts read bytes into
 *					buffer pointed to by *BufferPtr
 *
 *
 ******************************************************************************/
void Dataflash::Buffer_Read_Str (uint8_t BufferNo, unsigned int IntPageAdr, unsigned int No_of_bytes, uint8_t *BufferPtr)
{
    CHIP_SELECT

    if (1 == BufferNo) {					//read byte(s) from buffer 1
        SPI.transfer(Buf1Read);				//buffer 1 read op-code
    } else if (2 == BufferNo) {				//read byte(s) from buffer 2
        SPI.transfer(Buf2Read);				//buffer 2 read op-code
    }

    SPI.transfer(0x00);					//don't cares
    SPI.transfer((uint8_t)(IntPageAdr>>8));//upper part of internal buffer address
    SPI.transfer((uint8_t)(IntPageAdr));	//lower part of internal buffer address
    SPI.transfer(0x00);					//don't cares

    for(int i = 0; i < No_of_bytes; i++) {
        BufferPtr[i] = SPI.transfer(0x00);
    }
    CHIP_DESELECT

}

void Dataflash::Page_Read_Str (uint8_t PageAdr, unsigned int IntPageAdr, unsigned int No_of_bytes, uint8_t *BufferPtr)
{
    uint8_t high_byte = PageAdr >> (16 - PageBits);
    uint8_t mid_byte = PageAdr << (PageBits - 8) | (uint8_t)(IntPageAdr >> 8);
    uint8_t low_byte = (uint8_t)IntPageAdr;

    CHIP_SELECT
    SPI.transfer(FlashPageRead);

    //24 bit address
    //xxxxPPPPPPPPPPPBBBBBBBBB
    //P = page addr bits, B = starting byte addr bits
    SPI.transfer(high_byte);
    SPI.transfer(mid_byte);
    SPI.transfer(low_byte);

    //32bits of don't care
    SPI.transfer(DUMMY_BYTE);
    SPI.transfer(DUMMY_BYTE);
    SPI.transfer(DUMMY_BYTE);
    SPI.transfer(DUMMY_BYTE);

    for(int i = 0; i < No_of_bytes; i++) {
        BufferPtr[i] = SPI.transfer(DUMMY_BYTE);
    }

    CHIP_DESELECT
}

/*****************************************************************************
 *
 *
 *	Function name : Buffer_Write_Enable
 *
 *	Returns :		None
 *
 *	Parameters :	IntPageAdr	->	Internal page address to start writing from
 *			BufferAdr	->	Decides usage of either buffer 1 or 2
 *
 *	Purpose :	Enables continous write functionality to one of the Dataflash buffers
 *			buffers. NOTE : User must ensure that CS goes high to terminate
 *			this mode before accessing other Dataflash functionalities
 *
 ******************************************************************************/
void Dataflash::Buffer_Write_Enable (uint8_t BufferNo, unsigned int IntPageAdr)
{
    CHIP_SELECT
    if (1 == BufferNo) {			//write enable to buffer 1
        SPI.transfer(Buf1Write);			//buffer 1 write op-code
    } else if (2 == BufferNo) {			//write enable to buffer 2
        SPI.transfer(Buf2Write);			//buffer 2 write op-code
    }
    SPI.transfer(0x00);				//don't cares
    SPI.transfer((uint8_t)(IntPageAdr>>8));//upper part of internal buffer address
    SPI.transfer((uint8_t)(IntPageAdr));	//lower part of internal buffer address

    CHIP_DESELECT
}

/*****************************************************************************
 *
 *
 *	Function name : Buffer_Write_Str
 *
 *	Returns :		None
 *
 *	Parameters :	BufferNo	->	Decides usage of either buffer 1 or 2
 *			IntPageAdr	->	Internal page address
 *			No_of_bytes	->	Number of bytes to be written
 *			*BufferPtr	->	address of buffer to be used for copy of bytes
 *			from AVR buffer to Dataflash buffer 1 (or 2)
 *
 *	Purpose :		Copies one or more bytes to one of the Dataflash
 *				internal SRAM buffers from AVR SRAM buffer
 *			pointed to by *BufferPtr
 *
 ******************************************************************************/
void Dataflash::Buffer_Write_Str (uint8_t BufferNo, unsigned int IntPageAdr, unsigned int No_of_bytes, uint8_t *BufferPtr)
{
    CHIP_SELECT
    if (1 == BufferNo) {			//write byte(s) to buffer 1
        SPI.transfer(Buf1Write);			//buffer 1 write op-code
    } else if (2 == BufferNo) {			//write byte(s) to buffer 2
        SPI.transfer(Buf2Write);			//buffer 2 write op-code
    }
    SPI.transfer(0x00);				//don't cares
    SPI.transfer((uint8_t)(IntPageAdr >> 8));//upper part of internal buffer address
    SPI.transfer((uint8_t)(IntPageAdr));//lower part of internal buffer address
    for(int i = 0; i < No_of_bytes; i++) {
        SPI.transfer(BufferPtr[i]);
    }

    CHIP_DESELECT
}

/*****************************************************************************
 *
 *
 *	Function name : Buffer_To_Page
 *
 *	Returns :		None
 *
 *	Parameters :	BufferAdr	->	Decides usage of either buffer 1 or 2
 *			PageAdr		->	Address of flash page to be programmed
 *
 *	Purpose :	Transfers a page from Dataflash SRAM buffer to flash
 *
 *
 ******************************************************************************/
void Dataflash::Buffer_To_Page (uint8_t BufferNo, unsigned int PageAdr)
{
    CHIP_SELECT
    if (1 == BufferNo) {					//program flash page from buffer 1
        SPI.transfer(Buf1ToFlashWE);					//buffer 1 to flash with erase op-code
    } else if (2 == BufferNo) {					//program flash page from buffer 2
        SPI.transfer(Buf2ToFlashWE);					//buffer 2 to flash with erase op-code
    }

    SPI.transfer((uint8_t)(PageAdr >> (16 - PageBits)));	//upper part of page address
    SPI.transfer((uint8_t)(PageAdr << (PageBits - 8)));	//lower part of page address
    SPI.transfer(0x00);						//don't cares

    strobe();
    waitForReady(ONE_SECOND);
    CHIP_DESELECT
}

uint8_t Dataflash::readStatus() {
    SPI.transfer(StatusReg);		//send status register read op-code
    uint8_t result = SPI.transfer(0x00);			//dummy write to get result
    return result;				//return the read status register value
}


// Poll status register until busy flag is clear or timeout occurs
// Done within the context of a command, and thus does not select or unselect the chip
uint8_t Dataflash::waitForReady(uint32_t timeout) {
  uint8_t  status;
  uint32_t startTime = millis();

  do {
    status = readStatus();
    if((millis() - startTime) > timeout) {
      //Serial.print("waitForReady timeout; "); Serial.println(timeout);
      return false;
    }
  } while(!(status & READY_BIT));

  //Serial.print("[DEBUG] Dataflash status: "); Serial.println(status, BIN);

  return status;
}

void Dataflash::chipSelect() {
  SPI.beginTransaction(dflashSettings);
  if (digitalRead(SS) == LOW) {
    //Serial.print(F("Dataflash: SS was already low, an error state"));
    digitalWrite(SS, HIGH);
  }

  //Super hacky; shut off BLE chip select
  digitalWrite(6, HIGH);
  digitalWrite(SS, LOW);
}

void Dataflash::chipDeselect() {
  digitalWrite(SS, HIGH);
  SPI.endTransaction();
}

void Dataflash::strobe() {
  digitalWrite(SS, HIGH);
  digitalWrite(SS, LOW);
}

// *****************************[ End Of DATAFLASH.C ]*************************

