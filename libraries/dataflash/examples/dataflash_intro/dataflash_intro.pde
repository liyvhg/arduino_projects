#include <dataflash.h>

#define PAGE 1
#define PRIMARY_BUFFER 1

Dataflash dflash;

void setup()
{
    Serial.begin(115200);
    while(!Serial);
    Serial.println("Lets begin!");
    dflash.init(); //initialize the memory (pins are defined in dataflash.cpp
}

void loop()
{

    //Copy some text (including something like the time) into a page

    char messageline[100] = "when this was written, millis was: ";
    char time[64];
    sprintf(time,"%d",millis());
    strncat(messageline, time, 64);
    int len = strlen(messageline);

    dflash.Buffer_Write_Str(PRIMARY_BUFFER, PAGE, len+1, (unsigned char*)messageline);

    Serial.println(messageline);
    dflash.Buffer_To_Page(PRIMARY_BUFFER, PAGE); //write the buffer to the memory on page: lastpage

    delay(1000);

    //print it out
    char readBuffer[100];

    dflash.Page_To_Buffer(PRIMARY_BUFFER, PAGE);
    dflash.Buffer_Read_Str(PRIMARY_BUFFER, PAGE, len+1, (unsigned char*)readBuffer);
    Serial.print("The current millis is: ");
    Serial.print(millis());
    Serial.println("");
    Serial.println(readBuffer);


}
