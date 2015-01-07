
// Import libraries (BLEPeripheral depends on SPI)
#include <SPI.h>
#include <BLEPeripheral.h>
#include <dataflash.h>
#include <Bounce2.h>
#include "VirtualPortal.h"
//#include "NavSwitch.h"
//#include <MemoryFree.h>

// define pins (varies per shield/board)
#define BLE_REQ   6
#define BLE_RDY   7
#define BLE_RST   4

#define LED_PIN   9

//NavSwitch nav = NavSwitch(8, 5, 3);

// create peripheral instance, see pinouts above
BLEPeripheral blePeripheral = BLEPeripheral(BLE_REQ, BLE_RDY, BLE_RST);

// create one or more services
BLEService service = BLEService("533E15303ABEF33FCD00594E8B0A8EA3");
BLEService shortService = BLEService("1530");

// create one or more characteristics
BLECharacteristic txCharacteristic = BLECharacteristic("533E15423ABEF33FCD00594E8B0A8EA3", BLERead | BLENotify, BLE_ATTRIBUTE_MAX_VALUE_LENGTH);
BLECharacteristic rxCharacteristic = BLECharacteristic("533E15433ABEF33FCD00594E8B0A8EA3", BLEWrite, BLE_ATTRIBUTE_MAX_VALUE_LENGTH);

VirtualPortal vp = VirtualPortal();

unsigned char len = 0;

long previousMillis = 0;        // will store last time LED was updated
long interval = 1000;           // interval at which to blink (milliseconds)
bool subscribed = false;

int libraryId = 0; //Token being displayed
char topline[BLOCK_SIZE] = {0};
char bottomline[BLOCK_SIZE] = {0};

void setup() {
    Serial.begin(115200);
    delay(3000);  //3 seconds delay for enabling to see the start up comments on the serial board

    pinMode(LED_PIN, OUTPUT);

    //nav.init();

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

    //Serial.println(F("BLE Portal Peripheral"));
}

void loop() {
  blePeripheral.poll();
  //int update = nav.update();
  unsigned long currentMillis = millis();

  if(ble_busy()) { return; }


  if (subscribed) {
    //Maybe limit this to $interval if its taking too many cycles
    analogWrite(LED_PIN, vp.light());

  }//end subscribed

  /*
  //Look for navigation
  if (update) {
    Serial.println("Nav switch update!");
    NavSwitch::NavDir direction = nav.read();
    switch (direction) {
      case NavSwitch::ONE: //up?  down?
        //Token::display(libraryId, topline, bottomline);
        Serial.println("Activated nav switch one");
        break;
      case NavSwitch::TWO:
        Serial.println("Activated nav switch two");
        //Token::display(libraryId, topline, bottomline);
        break;
      case NavSwitch::TEE: //Select
        Serial.println("Activated nav switch select");
        //vp.loadToken(new Token(libraryId));
        break;
    } //end switch
  }//end update
  */

  //Do something every interval
  if(currentMillis - previousMillis > interval) {
    previousMillis = currentMillis;

    /* //1073 during my last check
    Serial.print("freeMemory()=");
    Serial.println(freeMemory());
    */
  }

  if (Serial.available() > 0) {
    // read the incoming byte:
    int incomingByte = Serial.read();
    switch (incomingByte) {
#ifdef TOKEN_IMPORT
      case 'I': //import
        Token::import();
        break;
#endif
      case 'L': //Load
        Serial.println(F("Loading token 1 (hardcoded)"));
        vp.loadToken(new Token(1));
        break;
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
  Serial.print(F("Subscribed to "));
  Serial.println(characteristic.uuid());
  subscribed = true;
}

void unsubscribeHandler(BLECentral& central, BLECharacteristic& characteristic)
{
  Serial.print(F("Unsubscribed to "));
  Serial.println(characteristic.uuid());
  subscribed = false;
}

void writeHandler(BLECentral& central, BLECharacteristic& characteristic)
{
    unsigned char len = characteristic.valueLength();
    uint8_t *val = (uint8_t*)characteristic.value();
    uint8_t response[BLE_ATTRIBUTE_MAX_VALUE_LENGTH] = {0};

    len = vp.respondTo(val, response);

    //respond if data to respond with
    if (len > 0) {
      txCharacteristic.setValue(response, BLE_ATTRIBUTE_MAX_VALUE_LENGTH);
    }
}

unsigned char ble_busy() {
    if(digitalRead(BLE_REQ) == HIGH) {
        return 0;
    } else {
        return 1;
    }
}
