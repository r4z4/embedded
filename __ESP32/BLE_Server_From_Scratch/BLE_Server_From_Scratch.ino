/*
https://www.youtube.com/watch?v=lj99N6pW-Ew (15:00)
TODO

    [] Create a BLE Device
        [] Import BLEDevice Library
        [] Name Device
        [] Initialize Device
        
    [] Create Server
        [] Import BLEServer Library
        [] Create BLEServer
   
    [] Create Server Callbacks
        [] OnConnect
            [] Turn on LED
        [] OnDisconnect
            [] Turn off LED
    
    [] Create Service
        [] Define Service UUID
        [] Create a Service
        [] Start the Service

    [] Create Characteristic
        [] Define Characteristic UUID
        [] Create a Characteristic
        [] Add Characteristic to Service

    [] Create Characteristic Callback
        [] Create a sub-class to override BLECharacteristicCallbacks
        [] Send millis value every time request comes in

    [] Create Descriptor
        [] Add a Descriptor (2901)

    [] Advertise the Service
        [] Get advertising object from server
        [] Set the properties of the Advertisement Data
        [] Set the Advertisement Data to the Service
        [] Add Service UUID
        [] Start Advertising
*/

#include <BLEDevice.h>
#include <BLE2901.h>

#define DEVICE_NAME "CIA Monitoring Device"
#define SERVICE_1_UUID "63fe1426-5be8-4956-8e4e-4a3f829e6681"
#define CHARACTERISTIC_1A_UUID "0360b78e-0c75-4f9c-b613-92962ecf2553"

class MyServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer *pServer) {
        digitalWrite(2, HIGH); // On his ESP32 he has a LED an digital pin 2
        Serial.println("Client Connected");
    }

    void onDisconnect(BLEServer *pServer) {
        digitalWrite(2, LOW);
        Serial.println("Client Disconnected");
        BLEDevice::startAdvertising();
    }
};

class MyCharacteristicCallbacks : public BLECharacteristicCallbacks {
    void onRead(BLECharacteristic *pCharacteristic) {
        uint32_t currentMillis = millis() / 1000; // - /1000 to get seconds
    }
};

void setup() {
    Serial.begin(9600);
    Serial.println("ESP32 BLE Server setup starting...");

    // Pin Modes
    pinMode(2, OUTPUT);

    // Init Device
    BLEDevice::init(DEVICE_NAME);

    // Create Server
    BLEServer *pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());

    // Services
    BLEService *pService = pServer->createService(SERVICE_1_UUID);

    // Characteristics
    BLECharacteristic *pCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_1A_UUID,
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE
    );

    pCharacteristic->setCallbacks(new MyCharacteristicCallbacks());

    BLE2901 *pDescriptor_2901 = new BLE2901();
    pDescriptor_2901->setDescription("Time");
    pCharacteristic->addDescriptor(pDescriptor_2901);

    pService->start();

    // Start Advertising
    // Get Advertising Object
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();

    BLEAdvertisementData advertisementData; // Passive Scan
    advertisementData.setName(DEVICE_NAME); // Need this so that ALL scanners can see it
    advertisementData.setManufacturerData("Chinese Factory");

    BLEAdvertisementData scanResponseData; // Active Scan
    scanResponseData.setName("Extra Scan Data");

    pAdvertising->setAdvertisementData(advertisementData);
    pAdvertising->setScanResponseData(scanResponseData);

    pAdvertising->addServiceUUID(SERVICE_1_UUID);
    pAdvertising->setScanResponse(true);

    BLEDevice::startAdvertising();
}

void loop() {
  // put your main code here, to run repeatedly:

}
