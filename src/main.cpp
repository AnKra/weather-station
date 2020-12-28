#include <BLEAddress.h>
#include <BLEAdvertisedDevice.h>
#include <BLEDevice.h>
#include <BLEScan.h>
#include <BLEUtils.h>
#include <SPI.h>
#include <SPIFFS.h>
#include <TFT_eSPI.h>

#include "hal/Settings.h"
#include "hal/wifiManager.h"
#include "ruuvi/bluetooth_listener.h"
#include "tft/colors.h"
#include "tft/graph.h"

// general
bool setup_successful = false;

// bluetooth
const int scan_time = 100;
BLEScan *ble_scan;

// display
weather_station::tft::Graph *graph;

void setup() {
  Serial.begin(115200);

  // Load Settings from flash
  hal::Settings settings;
  if (!SPIFFS.begin(true)) {
    printf("ERROR: SPIFFS mount failed.\n");
    return;
  }
  settings.load();
  Serial.println("Using Settings:");
  settings.print();

  // Start WiFi
  hal::setupWifi();
  hal::startWifi(settings.getSsid().c_str(), settings.getPassword().c_str());

  // bluetooth
  std::function<void(double x, double y)> draw_function = [](const double x, const double y) {
    assert(graph);
    graph->drawPixel(x, y, YELLOW);
  };

  Serial.println("Scanning...");
  BLEDevice::init("");
  ble_scan = BLEDevice::getScan();
  ble_scan->setAdvertisedDeviceCallbacks(new weather_station::ruuvi::BluetoothListener(draw_function));
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

  graph = new weather_station::tft::Graph(width, height, x_min, x_max, cell_width, y_min, y_max, cell_height);
  graph->drawAxes(title, x_label, y_label);

  setup_successful = true;
}

void printLocalTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("WARNING: Local Time not available.");
  } else {
    Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  }
}

void loop() {
  if (!setup_successful) {
    Serial.println("ERROR: Setup failed. Check for error messages.");
    return;
  }

  try {
    printLocalTime();  // Demo NTP timing
    ble_scan->start(scan_time, false);
  } catch (const std::runtime_error &e) {
    Serial.println(e.what());
  }
  ble_scan->clearResults();
}
