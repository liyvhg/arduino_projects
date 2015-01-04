
#include <SPI.h>
#include <dataflash.h>

#define PAGE 1
#define PRIMARY_BUFFER 1
#define PAGE_START 0
#define TEST_STRING_MAX 100

Dataflash dflash;

void setup() {
    Serial.begin(115200);
    while(!Serial);
    Serial.println("Lets begin!");
    Serial.print("Using Slave select "); Serial.println(SS, DEC);
    dflash.init();
}


#define REQN 6
void loop() {
    Serial.println("******    Loop    ******");

    //BLE busy
#ifdef BLEND_MICRO
    while(digitalRead(REQN) == LOW);
#endif

    //Copy some dynamic text into a page
    char writeBuffer[TEST_STRING_MAX] = "when this was written, millis was: ";
    char time[64] = {0};
    sprintf(time,"%d",millis());
    strncat(writeBuffer, time, 64);

    dflash.Page_To_Buffer(PAGE, PRIMARY_BUFFER);
    dflash.Buffer_Write_Str(PRIMARY_BUFFER, PAGE_START, TEST_STRING_MAX, (unsigned char*)writeBuffer);
    dflash.Buffer_To_Page(PRIMARY_BUFFER, PAGE);

    Serial.println("Saved a string to memory, sleeping 1000ms");
    delay(1000);

    //print it out
    char readBuffer[TEST_STRING_MAX] = {0};

    dflash.Page_To_Buffer(PAGE, PRIMARY_BUFFER);
    dflash.Buffer_Read_Str(PRIMARY_BUFFER, PAGE_START, TEST_STRING_MAX, (unsigned char*)readBuffer);

    Serial.print("The current millis is: ");
    Serial.print(millis());
    Serial.println("");

    Serial.print("Here is the data I saved: ");
    for(int i = 0; i < TEST_STRING_MAX; i++) {
      Serial.print(readBuffer[i], HEX);
    }
    Serial.println(" ");

    delay(5000);
}
