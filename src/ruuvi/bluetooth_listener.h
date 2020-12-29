#ifndef __RUUVI__BLUETOOTH_LISTENER_H__
#define __RUUVI__BLUETOOTH_LISTENER_H__

#include <functional>

#include <BLEAddress.h>
#include <BLEAdvertisedDevice.h>
#include <BLEDevice.h>
#include <BLEScan.h>
#include <BLEUtils.h>

#include "ruuvi/ruuvi_decoder.h"

namespace weather_station {
namespace ruuvi {

class BluetoothListener : public BLEAdvertisedDeviceCallbacks {
 public:
  BluetoothListener(std::function<void(double, double)> draw_function) { draw_function_ = draw_function; }

  void onResult(BLEAdvertisedDevice advertised_device) {
    if (advertised_device.getAddress().toString() == mac_address_) {
      auto raw_data = convertToHex(advertised_device.getManufacturerData());
      decodeData(raw_data, temperature_, humidity_, pressure_);
      rssi_ruuvi_ = advertised_device.getRSSI();
      printData(advertised_device.toString());
      draw_function_(++n_received_measurements_, temperature_);
    }
  }

 private:
  static std::string convertToHex(const std::string &manufacturer_data) {
    return BLEUtils::buildHexData(nullptr, (uint8_t *)manufacturer_data.data(), manufacturer_data.length());
  }

  void printData(const std::string &device_label) const {
    Serial.println(device_label.c_str());
    Serial.print("temp: ");
    Serial.print(temperature_);
    Serial.println("°C");
    Serial.print("hum: ");
    Serial.print(humidity_);
    Serial.println("%");
    Serial.print("pressure: ");
    Serial.println(pressure_);
  }

  std::string mac_address_ = "f8:56:bb:f1:d5:99";

  int voltage_power_;
  double temperature_;
  double humidity_;
  double pressure_;
  double rssi_ruuvi_;  // signal strength

  int n_received_measurements_ = 0;
  std::function<void(double, double)> draw_function_;
};

}  // namespace ruuvi
}  // namespace weather_station

#endif