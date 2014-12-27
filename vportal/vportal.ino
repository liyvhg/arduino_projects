
#include <SPI.h>
#include <boards.h>
#include "blend.h"
#include "Boards.h"

void setup() {
    Serial.begin(57600);
    Serial.println("Setup");

    // Init. and start BLE library.
    ble_begin();
}

void loop()
{

    ble_do_events();

}

