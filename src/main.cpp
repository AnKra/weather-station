#include <BLEAddress.h>
#include <BLEAdvertisedDevice.h>
#include <BLEDevice.h>
#include <BLEScan.h>
#include <BLEUtils.h>
#include <SPI.h>
#include <SPIFFS.h>
#include <TFT_eSPI.h>

#include <algorithm>

#include "bluetooth_listener.h"
#include "colors.h"
#include "graph.h"
#include "hal/Settings.h"

// bluetooth
int scanTime = 100;  // In seconds
BLEScan *pBLEScan;

// display
weather_station::Graph *graph;

// settings
hal::Settings settings;

void setup() {
  Serial.begin(115200);

  // Load Settings from flash
  if (SPIFFS.begin(true)) {
    printf("SPIFFS mount succeeded.\n");
    settings.load();
  } else {
    printf("SPIFFS mount failed.\n");
  }
  Serial.println("Using Settings:");
  settings.print();

  // bluetooth
  std::function<void(double x, double y)> draw_function = [](const double x,
                                                             const double y) {
    assert(graph);
    graph->drawPixel(x, y, YELLOW);
  };

  Serial.println("Scanning...");
  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(
      new weather_station::BluetoothListener(draw_function));
  pBLEScan->setActiveScan(
      true);  // active scan uses more power, but get results faster
  pBLEScan->setInterval(1);
  pBLEScan->setWindow(1);  // less or equal setInterval value

  // display
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

  graph = new weather_station::Graph(width, height, x_min, x_max, cell_width,
                                     y_min, y_max, cell_height);
  graph->drawAxes(title, x_label, y_label);
}

void loop() {
  try {
    pBLEScan->start(scanTime, false);
  } catch (const std::runtime_error &e) {
    Serial.println(e.what());
  }
  pBLEScan->clearResults();
}
