#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <driver/adc.h>
#include <BLE2902.h>
#include "car_motors.h"

BLECharacteristic *pCharacteristic;

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID        "751bf727-d925-45a1-9f57-93bed09e7122"
#define CHARACTERISTIC_UUID "d7ae6086-2b6e-421a-a631-dd1df37bf44d"

class JoyStickCallback: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      uint8_t* rxValue = pCharacteristic->getData();

      unsigned int xval = (rxValue[0] * 1000) + (rxValue[1] * 100) + (rxValue[2] * 10) + rxValue[3];
      unsigned int yval = (rxValue[4] * 1000) + (rxValue[5] * 100) + (rxValue[6] * 10) + rxValue[7];
      int swval = rxValue[8];

      CM.updateCarMotors(xval, yval, swval);
    }
};

void setup() {

  Serial.begin(115200);
  Serial.println("Starting BLE work!");

  CM.setupCarMotors();

  BLEDevice::init("Car");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE  |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );
  pCharacteristic->addDescriptor(new BLE2902());
  pCharacteristic->setCallbacks(new JoyStickCallback());

  pService->start();
  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->addServiceUUID(pService->getUUID());
  pAdvertising->start();
}

void loop() {
  delay(1000);
}
