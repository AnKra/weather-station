#include <BLEAddress.h>
#include <BLEAdvertisedDevice.h>
#include <BLEDevice.h>
#include <BLEScan.h>
#include <BLEUtils.h>
#include <SPI.h>
#include <SPIFFS.h>
#include <TFT_eSPI.h>

#include <ctime>
#include <memory>

#include "display/colors.h"
#include "display/graph.h"
#include "display/tft.h"
#include "hal/Settings.h"
#include "hal/wifiManager.h"
#include "ruuvi/bluetooth_listener.h"

// general
bool setup_successful = false;

// bluetooth
const int scan_time = 100;
BLEScan *ble_scan;

// display
weather_station::display::Graph *graph;

time_t getTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("WARNING: Local Time not available.");
  } else {
    Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  }

  return mktime(&timeinfo);
}

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
  std::function<void(float, float)> draw_function = [](const float temperature, const float /* humidity */) {
    assert(graph);
    graph->addDataPoint(getTime(), temperature, YELLOW);
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
  const String title = {"Balkon"};
  const String x_label = {"t"};
  const String y_label = {"°C"};

  std::unique_ptr<weather_station::display::Tft> tft = std::make_unique<weather_station::display::Tft>();
  graph = new weather_station::display::Graph(std::move(tft), width, height, title, x_label, y_label);
  graph->drawAxes();

  setup_successful = true;
}

void loop() {
  if (!setup_successful) {
    Serial.println("ERROR: Setup failed. Check for error messages.");
    return;
  }

  try {
    ble_scan->start(scan_time, false);
  } catch (const std::runtime_error &e) {
    Serial.println(e.what());
  }
  ble_scan->clearResults();
}
