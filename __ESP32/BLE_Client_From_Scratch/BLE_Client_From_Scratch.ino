#include <BLEDevice.h>

#define ONBOARD_LED 2

// Globals
static BLEUUID serviceUUIDTarget("63fe1426-5be8-4956-8e4e-4a3f829e6681");// From Server example
static BLEUUID charUUIDTarget("0360b78e-0c75-4f9c-b613-92962ecf2553"); // From Server example
static BLEAdvertisedDevice *pTargetDevice;

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    Serial.print("BLE Advertised Device found: ");
    Serial.println(advertisedDevice.toString());

    // Check if device we found has a service UUID, and it matches the one we want
    if (advertisedDevice.haveServiceUUID && advertisedDevice.isAdvertising(serviceUUIDTarget)) {
      Serial.println("Target Service Found");
      pTargetDevice = new BLEAdvertisedDevice(advertisedDevice);
    }
  }
};

class MyClientCallbacks : public BLEClientCallbacks {
  void onConnect(BLEClient *pClient) {
    Serial.println("Connected ");
    digitalWrite(ONBOARD_LED, HIGH);
  }

  void onDisconnect(BLEClient *pClient) {
    Serial.println("Disconnected ");
    digitalWrite(ONBOARD_LED, LOW);
  }
};

void setup() {
  Serial.begin(9600);
  Serial.println();

  pinMode(ONBOARD_LED, OUTPUT);

  // BLE Device
  BLEDevice::init("");

  // Scan
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);
  // start() is a blocking function. Program stops for 5 seconds.
  pBLEScan->start(5, false); // 5 = # of seconds to perform scan. 2nd param = continuous scan or not. False = just scan once.

  // Client
  BLEClient* pClient = BLEDevice::createClient();
  pClient->setClientCallbacks(new MyClientCallbacks());
  pClient->Connect(pTargetDevice);
  Serial.print("Connect to ");
  Serial.println(targetDevice->getName());

  // Show characteristic value
  BLERemoteService * pRemoteService = pClient->getService(serviceUUIDTarget);
  if (pRemoteService == nullptr) {
    Serial.print("Failed to find our service UUID");
    pClient->disconnect();
  }

  BLERemoteCharacteristic * pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUIDTarget);
  if (pRemoteService == nullptr) {
    Serial.print("Failed to find our characteristic UUID");
    pClient->disconnect();
  }

  if (pRemoteCharacteristic->canRead()) {
    int value = pRemoteCharacteristic->readUInt32();
    Serial.print("The characteristic value was: ");
    Serial.println(value);
  }
}

void loop() {

}
