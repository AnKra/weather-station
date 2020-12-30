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
      draw_function_(temperature_, humidity_);
    }
  }

 private:
  static std::string convertToHex(const std::string &manufacturer_data) {
    return BLEUtils::buildHexData(nullptr, (uint8_t *)manufacturer_data.data(),
                                  static_cast<uint8_t>(manufacturer_data.length()));
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
  float temperature_;
  float humidity_;
  float pressure_;
  int32_t rssi_ruuvi_;  // signal strength

  std::function<void(float, float)> draw_function_;
};

}  // namespace ruuvi
}  // namespace weather_station

#endif