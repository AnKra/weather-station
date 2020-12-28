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
const int scan_time = 100;  // In seconds
BLEScan *ble_scan;

// display
weather_station::Graph *graph;

void setup() {
  Serial.begin(115200);

  // bluetooth
  std::function<void(double x, double y)> draw_function = [](const double x, const double y) {
    assert(graph);
    graph->drawPixel(x, y, YELLOW);
  };

  Serial.println("Scanning...");
  BLEDevice::init("");
  ble_scan = BLEDevice::getScan();
  ble_scan->setAdvertisedDeviceCallbacks(new weather_station::BluetoothListener(draw_function));
  ble_scan->setActiveScan(true);  // active scan uses more power, but get results faster
  ble_scan->setInterval(1);
  ble_scan->setWindow(1);  // less or equal setInterval value

  // display
  const int width = TFT_HEIGHT;
  const int height = TFT_WIDTH;
  const double x_min = -2;
  const double x_max = 20;
  const double cell_width = 2;
  const double y_min = -6;
  const double y_max = 40;
  const double cell_height = 2;
  const String title = {"Balkon"};
  const String x_label = {"t"};
  const String y_label = {"°C"};

  graph = new weather_station::Graph(width, height, x_min, x_max, cell_width, y_min, y_max, cell_height);
  graph->drawAxes(title, x_label, y_label);
}

void loop() {
  try {
    ble_scan->start(scan_time, false);
  } catch (const std::runtime_error &e) {
    Serial.println(e.what());
  }
  ble_scan->clearResults();
}
