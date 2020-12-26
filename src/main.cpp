#include <algorithm>

#include <BLEAddress.h>
#include <BLEAdvertisedDevice.h>
#include <BLEDevice.h>
#include <BLEScan.h>
#include <BLEUtils.h>

#include <SPI.h>
#include <TFT_eSPI.h>

#include "bluetooth_listener.h"
#include "colors.h"
#include "graph.h"

// bluetooth
int scanTime = 5; // In seconds
BLEScan *pBLEScan;

// display
TFT_eSPI *tft;
byte dp = 1;
int width = TFT_HEIGHT;
int height = TFT_WIDTH;
double x_min = -2;
double x_max = 20;
double cell_width = 2;
double y_min = -6;
double y_max = 40;
double cell_height = 2;
const String title = "Balkon";
const String x_label = "t";
const String y_label = "°C";

void setup() {
  Serial.begin(115200);

  // bluetooth
  std::function<void(double x, double y)> fn = [](const double x,
                                                  const double y) {
    drawPixel(*tft, x, y, width, height, x_min, x_max, cell_width, y_min, y_max,
              cell_height, YELLOW);
  };

  Serial.println("Scanning...");
  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new BluetoothListener(fn));
  pBLEScan->setActiveScan(
      true); // active scan uses more power, but get results faster
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99); // less or equal setInterval value

  // display
  tft = new TFT_eSPI();
  tft->begin();
  tft->fillScreen(BLACK);
  tft->setRotation(1);

  Graph(*tft, dp, width, height, x_min, x_max, cell_width, y_min, y_max,
        cell_height, title, x_label, y_label);
}

void loop() {
  BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
  pBLEScan
      ->clearResults(); // delete results fromBLEScan buffer to release memory
  delay(3000);
}
