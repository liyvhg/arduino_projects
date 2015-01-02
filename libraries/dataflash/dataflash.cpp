
#include "Dataflash.h"

//Hard coding for AT45DB041B (sorry!)
unsigned char PageBits = 11;
unsigned int  PageSize = 264;

Dataflash::Dataflash() : cs_pin(SS) {}

Dataflash::Dataflash(uint8_t cs_pin) : cs_pin(cs_pin) {}

Dataflash::~Dataflash() {
  SPI.end();
}

void Dataflash::init(void) {
    SPI.begin();
    // Resistor-based 5V->3.3V logic conversion is a little sloppy, so:
    SPI.setClockDivider(SPI_CLOCK_DIV8);
    SPI.setDataMode(SPI_MODE0);
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
void Dataflash::Page_To_Buffer (unsigned int PageAdr, unsigned char BufferNo) {
    if (1 == BufferNo) {			//transfer flash page to buffer 1
        SPI.transfer(FlashToBuf1Transfer);				//transfer to buffer 1 op-code
    } else if (2 == BufferNo) {					//transfer flash page to buffer 2
        SPI.transfer(FlashToBuf2Transfer);				//transfer to buffer 2 op-code
    }

    //24 byte addressing rrrrPPPP PPPPPPPx xxxxxxxx
    unsigned char high_byte = PageAdr >> 4;
    unsigned char mid_byte = PageAdr << 1;
    unsigned char low_byte = 0;

    SPI.transfer(high_byte);
    SPI.transfer(mid_byte);
    SPI.transfer(low_byte);

    waitForReady(ONE_SECOND);
}
/*****************************************************************************
 *
 *	Function name : Buffer_Read_Byte
 *
 *	Returns :		One read byte (any value)
 *
 *	Parameters :	BufferNo	->	Decides usage of either buffer 1 or 2
 *					IntPageAdr	->	Internal page address
 *
 *	Purpose :		Reads one byte from one of the Dataflash
 *					internal SRAM buffers
 *
 ******************************************************************************/
unsigned char Dataflash::Buffer_Read_Byte (unsigned char BufferNo, unsigned int IntPageAdr)
{
    unsigned char data = '0';

    if (1 == BufferNo) {			//read byte from buffer 1
        SPI.transfer(Buf1Read);			//buffer 1 read op-code
    } else if (2 == BufferNo) {			//read byte from buffer 2
        SPI.transfer(Buf2Read);					//buffer 2 read op-code
    }
    SPI.transfer(0x00);				//don't cares

    //24byte address
    SPI.transfer((unsigned char)(IntPageAdr >> 8));//upper part of internal buffer address
    SPI.transfer((unsigned char)(IntPageAdr));	//lower part of internal buffer address
    SPI.transfer(0x00);				//don't cares

    data = SPI.transfer(0x00);			//read byte

    return data;							//return the read data byte
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
void Dataflash::Buffer_Read_Str (unsigned char BufferNo, unsigned int IntPageAdr, unsigned int No_of_bytes, unsigned char *BufferPtr)
{
    unsigned int i;
    if (1 == BufferNo) {					//read byte(s) from buffer 1
        SPI.transfer(Buf1Read);				//buffer 1 read op-code
    } else if (2 == BufferNo) {				//read byte(s) from buffer 2
        SPI.transfer(Buf2Read);				//buffer 2 read op-code
    }

    SPI.transfer(0x00);					//don't cares
    SPI.transfer((unsigned char)(IntPageAdr>>8));//upper part of internal buffer address
    SPI.transfer((unsigned char)(IntPageAdr));	//lower part of internal buffer address
    SPI.transfer(0x00);					//don't cares
    for(int i = 0; i < No_of_bytes; i++) {
        *(BufferPtr) = SPI.transfer(0x00);		//read byte and put it in AVR buffer pointed to by *BufferPtr
        BufferPtr++;					//point to next element in AVR buffer
    }

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
void Dataflash::Buffer_Write_Enable (unsigned char BufferNo, unsigned int IntPageAdr)
{
    if (1 == BufferNo) {			//write enable to buffer 1
        SPI.transfer(Buf1Write);			//buffer 1 write op-code
    } else if (2 == BufferNo) {			//write enable to buffer 2
        SPI.transfer(Buf2Write);			//buffer 2 write op-code
    }
    SPI.transfer(0x00);				//don't cares
    SPI.transfer((unsigned char)(IntPageAdr>>8));//upper part of internal buffer address
    SPI.transfer((unsigned char)(IntPageAdr));	//lower part of internal buffer address

}

/*****************************************************************************
 *
 *	Function name : Buffer_Write_Byte
 *
 *	Returns :		None
 *
 *	Parameters :	IntPageAdr	->	Internal page address to write byte to
 *			BufferAdr	->	Decides usage of either buffer 1 or 2
 *			Data		->	Data byte to be written
 *
 *	Purpose :		Writes one byte to one of the Dataflash
 *					internal SRAM buffers
 *
 ******************************************************************************/
void Dataflash::Buffer_Write_Byte (unsigned char BufferNo, unsigned int IntPageAdr, unsigned char Data)
{
    if (1 == BufferNo) {			//write byte to buffer 1
        SPI.transfer(Buf1Write);			//buffer 1 write op-code
    } else if (2 == BufferNo) {			//write byte to buffer 2
        SPI.transfer(Buf2Write);			//buffer 2 write op-code
    }

    SPI.transfer(0x00);				//don't cares
    SPI.transfer((unsigned char)(IntPageAdr>>8));//upper part of internal buffer address
    SPI.transfer((unsigned char)(IntPageAdr));	//lower part of internal buffer address
    SPI.transfer(Data);				//write data byte

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
void Dataflash::Buffer_Write_Str (unsigned char BufferNo, unsigned int IntPageAdr, unsigned int No_of_bytes, unsigned char *BufferPtr)
{
    if (1 == BufferNo) {			//write byte(s) to buffer 1
        SPI.transfer(Buf1Write);			//buffer 1 write op-code
    } else if (2 == BufferNo) {			//write byte(s) to buffer 2
        SPI.transfer(Buf2Write);			//buffer 2 write op-code
    }
    SPI.transfer(0x00);				//don't cares
    SPI.transfer((unsigned char)(IntPageAdr>>8));//upper part of internal buffer address
    SPI.transfer((unsigned char)(IntPageAdr));	//lower part of internal buffer address
    for(int i = 0; i < No_of_bytes; i++) {
        SPI.transfer(*(BufferPtr));			//write byte pointed at by *BufferPtr to Dataflash buffer 1 location
        BufferPtr++;				//point to next element in AVR buffer
    }
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
void Dataflash::Buffer_To_Page (unsigned char BufferNo, unsigned int PageAdr)
{
    if (1 == BufferNo) {					//program flash page from buffer 1
        SPI.transfer(Buf1ToFlashWE);					//buffer 1 to flash with erase op-code
    } else if (2 == BufferNo) {					//program flash page from buffer 2
        SPI.transfer(Buf2ToFlashWE);					//buffer 2 to flash with erase op-code
    }

    SPI.transfer((unsigned char)(PageAdr >> (16 - PageBits)));	//upper part of page address
    SPI.transfer((unsigned char)(PageAdr << (PageBits - 8)));	//lower part of page address
    SPI.transfer(0x00);						//don't cares

    waitForReady(ONE_SECOND);
}

/*****************************************************************************
 *
 *	Function name : Page_Buffer_Compare
 *
 *	Returns :		0 match, 1 if mismatch
 *
 *	Parameters :	BufferAdr	->	Decides usage of either buffer 1 or 2
 *			PageAdr		->	Address of flash page to be compared with buffer
 *
 *	Purpose :	comparte Buffer with Flash-Page
 *   added by Martin Thomas, Kaiserslautern, Germany. This routine was not
 *   included by ATMEL
 *
 ******************************************************************************/
unsigned char Dataflash::Page_Buffer_Compare(unsigned char BufferNo, unsigned int PageAdr)
{

    if (1 == BufferNo) {
        SPI.transfer(FlashToBuf1Compare);
    } else if (2 == BufferNo) {
        SPI.transfer(FlashToBuf2Compare);
    }

    SPI.transfer((unsigned char)(PageAdr >> (16 - PageBits)));	//upper part of page address
    SPI.transfer((unsigned char)(PageAdr << (PageBits - 8)));	//lower part of page address

    uint8_t status = waitForReady(ONE_SECOND);
    return (status & COMPARE_BIT);
}

unsigned char Dataflash::Read_status (void) {
    unsigned char result, index_copy;
    result = SPI.transfer(StatusReg);		//send status register read op-code
    result = SPI.transfer(0x00);			//dummy write to get result
    return result;				//return the read status register value
}


// Poll status register until busy flag is clear or timeout occurs
uint8_t Dataflash::waitForReady(uint32_t timeout) {
  uint8_t  status;
  uint32_t startTime = millis();

  do {
    status = Read_status();
    if((millis() - startTime) > timeout) { Serial.println("waitForReady timeout"); return false; }
  } while(!(status & READY_BIT));

  Serial.print("Dataflash status: "); Serial.println(status, BIN);

  return status;
}

// *****************************[ End Of DATAFLASH.C ]*************************

