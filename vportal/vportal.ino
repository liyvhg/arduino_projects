
// Import libraries (BLEPeripheral depends on SPI)
#include <SPI.h>
#include <BLEPeripheral.h>
#include <dataflash.h>
#include <Bounce2.h>
#include "VirtualPortal.h"
#include "NavSwitch.h"
#include <MemoryFree.h>

// define pins (varies per shield/board)
#define BLE_REQ   6
#define BLE_RDY   7
#define BLE_RST   4


//Should trap led be a real led, or LCD backlight?
bool trap_led = true;

#define TRAP_LED_PIN   13

//serLCD
#define BACKLIGHT_CMD 0x7C
#define BACKLIGHT_BASE 0x80
#define BACKLIGHT_LEVELS 30

NavSwitch nav = NavSwitch(12, 11, 10, 9);

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

void setup() {
    LCD.begin(9600);
    LCD.write(0xFE);   //command flag
    LCD.write(0x01);   //clear command.
    LCD.write(BACKLIGHT_CMD);
    LCD.write(BACKLIGHT_BASE + BACKLIGHT_LEVELS - 1);

    Serial.begin(115200);
    delay(3000);  //3 seconds delay for enabling to see the start up comments on the serial board
    pinMode(TRAP_LED_PIN, OUTPUT);
    nav.init();

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
  int update = nav.update();
  unsigned long currentMillis = millis();

  if(ble_busy()) { return; }


  if (subscribed) {

    if (trap_led) {
      analogWrite(TRAP_LED_PIN, vp.light());
    } else {
      uint8_t level = vp.light() / (255 * BACKLIGHT_LEVELS);
      LCD.write(BACKLIGHT_CMD);
      LCD.write(BACKLIGHT_BASE + (BACKLIGHT_BASE - level));
    }
  }//end subscribed


  //Serial navigation
  if (Serial.available() > 0) {
    // read the incoming byte:
    int incomingByte = Serial.read();
    switch (incomingByte) {
      case '1':
        libraryId++;
        break;
      case '2':
        libraryId--;
        break;
      case 'T':
        //set some sort of load flag
        break;
#ifdef TOKEN_IMPORT
      case 'I': //import
        Token::import();
        break;
#endif

    }
  }

  //Look for navigation
  if (update) {
    NavSwitch::NavDir direction = nav.read();
    switch (direction) {
      case NavSwitch::ONE: //up?  down?
        //Token::display();
        Serial.println("\t\t\t\t\t\t\t\tone");
        break;
      case NavSwitch::TWO:
        Serial.println("\t\t\t\t\t\ttwo");
        //Token::display();
        break;
      case NavSwitch::TEE: //Select
        Serial.println("\t\t\t\t\t\t\tTee");
        //vp.loadToken(new Token(libraryId));
        break;
    } //end switch
  }//end update

  //If load flag, remove old token, set LCD text, add new one next second
  //vp.loadToken(new Token(libraryId));
  //else display
  //Token preview(libraryId);
  //preview.display();

  //Do something every interval
  if(currentMillis - previousMillis > interval) {
    previousMillis = currentMillis;

    //1073 during my last check
    //Serial.print("freeMemory()="); Serial.println(freeMemory());
  }

}

// callbacks
void connectCallback(BLECentral& central)
{
    Serial.println(F("Connected"));
    vp.connect();
}

void disconnectCallback(BLECentral& central)
{
    vp.connect();
    subscribed = false;
}

void subscribeHandler(BLECentral& central, BLECharacteristic& characteristic)
{
  Serial.println(F("Subscribed"));
  vp.subscribe();
  subscribed = true;
}

void unsubscribeHandler(BLECentral& central, BLECharacteristic& characteristic)
{
  vp.unsubscribe();
  subscribed = false;
}

void writeHandler(BLECentral& central, BLECharacteristic& characteristic)
{
    unsigned char len = characteristic.valueLength();
    uint8_t *val = (uint8_t*)characteristic.value();
    uint8_t response[BLE_ATTRIBUTE_MAX_VALUE_LENGTH] = {0};
    unsigned long start = millis();
    len = vp.respondTo(val, response);

    //Serial.print("respondTo took (ms): "); Serial.println(millis() - start);

    //respond if data to respond with
    if (len > 0) {
      txCharacteristic.setValue(response, BLE_ATTRIBUTE_MAX_VALUE_LENGTH);
    }
}

unsigned char ble_busy() {
    return (digitalRead(BLE_REQ) == LOW);
}
