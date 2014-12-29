
// Import libraries (BLEPeripheral depends on SPI)
#include <SPI.h>
#include <BLEPeripheral.h>
#include "VirtualPortal.h"
//#include <MemoryFree.h>


// define pins (varies per shield/board)
#define BLE_REQ   6
#define BLE_RDY   7
#define BLE_RST   4

// create peripheral instance, see pinouts above
BLEPeripheral blePeripheral = BLEPeripheral(BLE_REQ, BLE_RDY, BLE_RST);

// create one or more services
BLEService service = BLEService("533E15303ABEF33FCD00594E8B0A8EA3");
BLEService shortService = BLEService("1530");

// create one or more characteristics
BLECharacteristic txCharacteristic = BLECharacteristic("533E15423ABEF33FCD00594E8B0A8EA3", BLERead | BLENotify, BLE_ATTRIBUTE_MAX_VALUE_LENGTH);
BLECharacteristic rxCharacteristic = BLECharacteristic("533E15433ABEF33FCD00594E8B0A8EA3", BLEWrite, BLE_ATTRIBUTE_MAX_VALUE_LENGTH);

VirtualPortal vp = VirtualPortal(0); //LightPin, none yet

void setup()
{
    Serial.begin(115200);
    delay(3000);  //3 seconds delay for enabling to see the start up comments on the serial board

    blePeripheral.setDeviceName("Skylanders Portal\0");
    blePeripheral.setLocalName("Skylanders Portal\0");
    blePeripheral.setAdvertisedServiceUuid(shortService.uuid());

    // add attributes (services, characteristics, descriptors) to peripheral
    blePeripheral.addAttribute(service);
    blePeripheral.addAttribute(rxCharacteristic);
    blePeripheral.addAttribute(txCharacteristic);

    blePeripheral.setEventHandler(BLEConnected, connectCallback);
    blePeripheral.setEventHandler(BLEDisconnected, disconnectCallback);
    txCharacteristic.setEventHandler(BLESubscribed, subscribeHandler);
    txCharacteristic.setEventHandler(BLEUnsubscribed, unsubscribeHandler);
    rxCharacteristic.setEventHandler(BLEWritten, writeHandler);

    // begin initialization
    blePeripheral.begin();

    Serial.println(F("BLE Portal Peripheral"));
}

unsigned char buf[BLE_ATTRIBUTE_MAX_VALUE_LENGTH] = {0};
unsigned char len = 0;

long previousMillis = 0;        // will store last time LED was updated
long interval = 1000;           // interval at which to blink (milliseconds)
bool subscribed = false;


void loop() {
  blePeripheral.poll();
  unsigned long currentMillis = millis();

  if(subscribed && currentMillis - previousMillis > interval) {
    previousMillis = currentMillis;
    //Do something every interval


    //Testing character loading
    if ( Serial.available() ) {
        char cmd = Serial.read();
        if (cmd == 'L') {
          Serial.println("calling VirtualPortal loadToken");
          vp.loadToken();
        }
    }


  }
}

// callbacks
void connectCallback(BLECentral& central)
{
    Serial.print(F("Connected event, central: "));
    Serial.println(central.address());
}

void disconnectCallback(BLECentral& central)
{
    Serial.print(F("Disconnected event, central: "));
    Serial.println(central.address());
    subscribed = false;
}

void subscribeHandler(BLECentral& central, BLECharacteristic& characteristic)
{
  Serial.print("Subscribed to ");
  Serial.println(characteristic.uuid());
  subscribed = true;
}

void unsubscribeHandler(BLECentral& central, BLECharacteristic& characteristic)
{
  Serial.print("Unsubscribed to ");
  Serial.println(characteristic.uuid());
  subscribed = false;
}

void writeHandler(BLECentral& central, BLECharacteristic& characteristic)
{
    unsigned char len = characteristic.valueLength();
    const unsigned char *val = characteristic.value();
    uint8_t response[BLE_ATTRIBUTE_MAX_VALUE_LENGTH] = {0};

    Serial.println(" ");

    Serial.print("<= ");
    for(int i = 0; i < len; i++) {
      Serial.write(val[i]);
    }
    Serial.println(" ");


    len = vp.respondTo((uint8_t*)val, response);
    Serial.print("=> ");
    for(int i = 0; i < len; i++) {
      Serial.write(val[i]);
    }
    Serial.println(" ");

    //respond if data to respond with
    if (len > 0) {
      bool success = txCharacteristic.setValue(response, BLE_ATTRIBUTE_MAX_VALUE_LENGTH);
      if (success) {
        Serial.println("Responded successfully");
      }
    }

}


