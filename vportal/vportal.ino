
// Import libraries (BLEPeripheral depends on SPI)
#include <SPI.h>
#include <BLEPeripheral.h>

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
BLECharacteristic txCharacteristic = BLECharacteristic("533E15423ABEF33FCD00594E8B0A8EA3", BLENotify, 20);
BLECharacteristic rxCharacteristic = BLECharacteristic("533E15433ABEF33FCD00594E8B0A8EA3", BLEWriteWithoutResponse, 20);

void setup() {
  Serial.begin(115200);
  delay(3000);  //3 seconds delay for enabling to see the start up comments on the serial board

  blePeripheral.setDeviceName("Skylanders Portal\0");
  blePeripheral.setLocalName("Skylanders Portal\0");
  blePeripheral.setAdvertisedServiceUuid(shortService.uuid());

  // add attributes (services, characteristics, descriptors) to peripheral
  //blePeripheral.addAttribute(service);
  blePeripheral.addAttribute(shortService);
  blePeripheral.addAttribute(rxCharacteristic);
  blePeripheral.addAttribute(txCharacteristic);

  blePeripheral.setEventHandler(BLEConnected, connectCallback);
  blePeripheral.setEventHandler(BLEDisconnected, disconnectCallback);

  txCharacteristic.setEventHandler(BLESubscribed, subscribeHandler);
  rxCharacteristic.setEventHandler(BLEWritten, writeHandler);

  // begin initialization
  blePeripheral.begin();

  Serial.println(F("BLE Portal Peripheral"));
}

void loop() {
  BLECentral central = blePeripheral.central();

  if (central) {
    // central connected to peripheral
    Serial.print(F("Connected to central: "));
    Serial.println(central.address());

    while (central.connected()) {
      // central still connected to peripheral

    }

    // central disconnected
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
  }

}

// callbacks
void connectCallback(BLECentral& central) {
  Serial.print(F("Connected event, central: "));
  Serial.println(central.address());
}

void disconnectCallback(BLECentral& central) {
  Serial.print(F("Disconnected event, central: "));
  Serial.println(central.address());
}

void subscribeHandler(BLECentral& central, BLECharacteristic& characteristic) {
  Serial.println("subscribe callback");
}

void writeHandler(BLECentral& central, BLECharacteristic& characteristic) {
  Serial.print(F("Characteristic event, writen: "));
  Serial.println((char*)characteristic.value());
}

