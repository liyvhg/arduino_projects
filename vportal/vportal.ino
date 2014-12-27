// Import libraries (BLEPeripheral depends on SPI)
#include <SPI.h>
#include <BLEPeripheral.h>

// define pins (varies per shield/board)
#define BLE_REQ   6
#define BLE_RDY   7
#define BLE_RST   4

#define PORTAL_SERVICE_UUID_SHORT  "1530"
#define PORTAL_SERVICE_UUID "533E15303ABEF33FCD00594E8B0A8EA3"
#define READ_CHARACTERISTIC_UUID "533E15423ABEF33FCD00594E8B0A8EA3"
#define WRITE_CHARACTERISTIC_UUID "533E15433ABEF33FCD00594E8B0A8EA3"


// create peripheral instance, see pinouts above
BLEPeripheral blePeripheral = BLEPeripheral(BLE_REQ, BLE_RDY, BLE_RST);

// uuid's can be:
//   16-bit: "ffff"
//  128-bit: "19b10010e8f2537e4f6cd104768a1214" (dashed format also supported)

// create one or more services
BLEService service = BLEService(PORTAL_SERVICE_UUID);

// create one or more characteristics
BLECharCharacteristic read_characteristic = BLECharCharacteristic(READ_CHARACTERISTIC_UUID, BLERead | BLENotify);
BLECharCharacteristic write_characteristic = BLECharCharacteristic(WRITE_CHARACTERISTIC_UUID, BLEWrite);

void setup() {
  Serial.begin(115200);
#if defined (__AVR_ATmega32U4__)
  //Wait until the serial port is available (useful only for the Leonardo)
  //As the Leonardo board is not reseted every time you open the Serial Monitor
  while(!Serial) {}
  delay(5000);  //5 seconds delay for enabling to see the start up comments on the serial board
#endif

  blePeripheral.setLocalName("Skylanders Portal\0"); // optional
  blePeripheral.setAdvertisedServiceUuid(PORTAL_SERVICE_UUID_SHORT); // optional

  // add attributes (services, characteristics, descriptors) to peripheral
  blePeripheral.addAttribute(service);
  blePeripheral.addAttribute(read_characteristic);
  blePeripheral.addAttribute(write_characteristic);

  // begin initialization
  blePeripheral.begin();
}

void loop() {
  BLECentral central = blePeripheral.central();

  if (central) {
    // central connected to peripheral
    Serial.print(F("Connected to central: "));
    Serial.println(central.address());

    while (central.connected()) {
      // central still connected to peripheral
      if (write_characteristic.written()) {
        // central wrote new value to characteristic
        Serial.println(write_characteristic.value(), DEC);

      }
    }

    // central disconnected
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
  }
}
