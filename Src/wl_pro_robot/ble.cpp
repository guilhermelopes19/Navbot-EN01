#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <ArduinoJson.h>
#include "ble.h"

BLEServer *pServer = NULL;
BLECharacteristic * pTxCharacteristic;
bool deviceConnected = false;
bool oldDeviceConnected = false;

BleDataTypDef ble_rx;



// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID           "6E400011-B5A3-F393-E0A9-E50E24DCCA9E" // UART service UUID
#define CHARACTERISTIC_UUID_RX "6E400012-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400013-B5A3-F393-E0A9-E50E24DCCA9E"

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
    uint8_t* rxData = pCharacteristic->getData();
    ble_rx.state = 1;
    if (pCharacteristic->getLength() == 20)
    {
        ble_rx.data.header[0]  = *(rxData+0);
        ble_rx.data.header[1]  = *(rxData+1);
        ble_rx.data.idle1[0]   = *(rxData+2);
        ble_rx.data.idle1[1]   = *(rxData+3);
        ble_rx.data.idle1[2]   = *(rxData+4);
        ble_rx.data.roll       = *(rxData+5);
        ble_rx.data.height     = *(rxData+6);
        ble_rx.data.linear_H   = *(rxData+7);
        ble_rx.data.linear_L   = *(rxData+8);
        ble_rx.data.angular    = *(rxData+9);
        ble_rx.data.stable     = *(rxData+10);
        ble_rx.data.mode       = *(rxData+11);
        ble_rx.data.dir        = *(rxData+12);
        ble_rx.data.joy_y      = *(rxData+13);
        ble_rx.data.joy_x      = *(rxData+14);
        ble_rx.data.idle2[0]   = *(rxData+15);
        ble_rx.data.idle2[1]   = *(rxData+16);
        ble_rx.data.idle2[2]   = *(rxData+17);
        ble_rx.data.idle2[3]   = *(rxData+18);
        ble_rx.data.checkSum   = *(rxData+19);
      }

    }
};

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};


void ble_init(char* botName) 
{
  // Create the BLE Device
  BLEDevice::init(botName);
  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);
  // Create a BLE Characteristic
  pTxCharacteristic = pService->createCharacteristic(
										CHARACTERISTIC_UUID_TX,
										BLECharacteristic::PROPERTY_NOTIFY
									);
  
  pTxCharacteristic->addDescriptor(new BLE2902());

  BLECharacteristic * pRxCharacteristic = pService->createCharacteristic(
											 CHARACTERISTIC_UUID_RX,
											BLECharacteristic::PROPERTY_WRITE
										);

  pRxCharacteristic->setCallbacks(new MyCallbacks());
  // Start the service
  pService->start();
  // Start advertising
  pServer->getAdvertising()->start();
  Serial.println("Waiting a client connection to notify...");
}
void ble_send_data(uint8_t* data, uint8_t len)
{
  pTxCharacteristic->setValue(data, len);  //reply
  pTxCharacteristic->notify();
}

void bleUartTest(void) 
{
  while(1)
  {

  }
}