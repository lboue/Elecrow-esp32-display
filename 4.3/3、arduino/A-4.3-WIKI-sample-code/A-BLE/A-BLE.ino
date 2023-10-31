#include "BLEDevice.h"              //BLE Driver Library
#include "BLEServer.h"              //BLE Bluetooth Server Library
#include "BLEUtils.h"               //BLE Utility Library
#include "BLE2902.h"                //Feature Addition Descriptor Library
#include <BLECharacteristic.h>      //BLE Feature Function Library
BLEAdvertising* pAdvertising = NULL;
BLEServer* pServer = NULL;
BLEService *pService = NULL;
BLECharacteristic* pCharacteristic = NULL;
#define bleServerName "ESP32SPI-BLE"  //Name of the BLE server
#define SERVICE_UUID "6479571c-2e6d-4b34-abe9-c35116712345"  //UUID of the service
#define CHARACTERISTIC_UUID "826f072d-f87c-4ae6-a416-6ffdcaa02d73"


bool connected_state = false;   //Creating a Device Connection Identifier

class MyServerCallbacks: public BLEServerCallbacks  //Create connect and disconnect calling classes
{
    void onConnect(BLEServer *pServer)//Start connection function
    {
      connected_state = true;   //Equipment is properly connected
    }
    void onDisconnect(BLEServer *pServer)//Disconnect Function
    {
      connected_state = false;  //Device connection error
    }

};
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  //BLE
  BLEDevice::init(bleServerName);  //Create BLE and set the name
  pServer = BLEDevice::createServer();  //Creating a BLE server
  pServer->setCallbacks(new MyServerCallbacks());  //Setting up the connect and disconnect calling classes
  pService = pServer->createService(SERVICE_UUID); //Creating BLE service

  pCharacteristic = pService->createCharacteristic(  //Create ble feature（Characterristic_UUID）
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY);
  pCharacteristic->setValue("ELECROW");
  //start broadcasting
  pAdvertising = BLEDevice::getAdvertising();  //A bleadvertising class pointer padvertising is defined, which points to bledevice:: getadvertising()
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->start();  //Start broadcasting
  pService->start();
  //  pAdvertising->stop();  //stop broadcasting
  //  pService->stop();      //Discontinuation of services
}

void loop() {

}
