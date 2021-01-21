#include <cstdlib>
#include <string>
#include "BLEDevice.h"

#define JoyStick_Vx 33
#define JoyStick_Vy 32
#define JoyStick_SW 22

// The remote service we wish to connect to.
static BLEUUID serviceUUID("751bf727-d925-45a1-9f57-93bed09e7122");
// The characteristic of the remote service we are interested in.
static BLEUUID    charUUID("d7ae6086-2b6e-421a-a631-dd1df37bf44d");

static BLEAddress *pServerAddress;
static boolean doConnect = false;
static boolean connected = false;

static BLERemoteCharacteristic* pRemoteCharacteristic;

// this gets called when we receive a message
// Note that the server has to call notify to trigger this
static void notifyCallback(
  BLERemoteCharacteristic* pBLERemoteCharacteristic,
  uint8_t* pData,
  size_t length,
  bool isNotify)
{

}

bool connectToServer(BLEAddress pAddress)
{
  Serial.print("Forming a connection to ");
  Serial.println(pAddress.toString().c_str());

  BLEClient*  pClient = BLEDevice::createClient();
  Serial.println(" - Created client");

  // Connect to the remote BLE Server.
  pClient->connect(pAddress);
  Serial.println(" - Connected to server");

  // Obtain a reference to the service we are after in the remote BLE server.
  BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
  if (pRemoteService == nullptr) {
    Serial.print("Failed to find our service UUID: ");
    Serial.println(serviceUUID.toString().c_str());
    return false;
  }
  Serial.println(" - Found our service");


  // Obtain a reference to the characteristic in the service of the remote BLE server.
  pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
  if (pRemoteCharacteristic == nullptr) {
    Serial.print("Failed to find our characteristic UUID: ");
    Serial.println(charUUID.toString().c_str());
    return false;
  }
  Serial.println(" - Found our characteristic");

  // Bind the notifyCallback method to the characteristic
  pRemoteCharacteristic->registerForNotify(notifyCallback);
  Serial.println(" - Binding to characteristic");
}

/**
   Scan for BLE servers and find the first one that advertises the service we are looking for.
*/
class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
    /**
        Called for each advertising BLE server.
    */
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      Serial.print("BLE Advertised Device found: ");
      Serial.println(advertisedDevice.toString().c_str());

      // We have found a device, let us now see if it contains the service we are looking for.
      if (advertisedDevice.haveServiceUUID() && advertisedDevice.getServiceUUID().equals(serviceUUID)) {

        Serial.print("Found our device!  address: ");
        advertisedDevice.getScan()->stop();

        pServerAddress = new BLEAddress(advertisedDevice.getAddress());
        doConnect = true;

      }
    }
};


void setup() {
  Serial.begin(115200);
  Serial.println("Starting Arduino BLE Client application...");

  pinMode(JoyStick_SW, INPUT_PULLUP);
  digitalWrite(JoyStick_SW, HIGH);

  BLEDevice::init("Joy");

  // Retrieve a Scanner and set the callback we want to use to be informed when we
  // have detected a new device.  Specify that we want active scanning and start the
  // scan to run for 30 seconds.
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);
  pBLEScan->start(30);

}

void loop() {
  // If the flag "doConnect" is true then we have scanned for and found the desired
  // BLE Server with which we wish to connect.  Now we connect to it.  Once we are
  // connected we set the connected flag to be true.
  if (doConnect == true) {
    if (connectToServer(*pServerAddress)) {
      Serial.println("We are now connected to the BLE Server.");
      connected = true;
    }
    else {
      Serial.println("We have failed to connect to the server; there is nothing more we will do.");
    }
    doConnect = false;
  }

  if (connected) {
    Serial.print("Sending: ");
    unsigned int x = analogRead(JoyStick_Vx);
    unsigned int y = analogRead(JoyStick_Vy);
    int sw = digitalRead(JoyStick_SW);

    // x, y range between 0 to 4095 (12 bit resolution)
    // sw ranges between 0, 1
    // Encode x, y, sw digit by digit into the buffer
    uint8_t buffer[9];

    for (int i = 3; i > -1; i--) {
      buffer[i] = x % 10;
      x = x / 10;
    }
    for (int j = 7; j > 3; j--) {
      buffer[j] = y % 10;
      y = y / 10;
    }
    buffer[8] = sw;

    pRemoteCharacteristic->writeValue(buffer, 9, false);
  }

  delay(50);
}
