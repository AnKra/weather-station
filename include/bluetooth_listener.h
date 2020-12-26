#include <functional>

#include <BLEAddress.h>
#include <BLEAdvertisedDevice.h>
#include <BLEDevice.h>
#include <BLEScan.h>
#include <BLEUtils.h>

class BluetoothListener : public BLEAdvertisedDeviceCallbacks {
public:
  BluetoothListener(std::function<void(double, double)> draw_function) {
    m_draw_function = draw_function;
  }

  void onResult(BLEAdvertisedDevice advertisedDevice) {
    if (MAC_add.indexOf(advertisedDevice.getAddress().toString().c_str()) >=
        0) {
      String raw_data = String(BLEUtils::buildHexData(
          nullptr, (uint8_t *)advertisedDevice.getManufacturerData().data(),
          advertisedDevice.getManufacturerData().length()));
      raw_data.toUpperCase();
      decodeRuuvi(raw_data, advertisedDevice.getRSSI());

      Serial.println(advertisedDevice.toString().c_str());
      Serial.print("temp: ");
      Serial.print(m_temperature);
      Serial.println("°C");
      Serial.print("hum: ");
      Serial.print(m_humidity);
      Serial.println("%");

      m_draw_function(++m_i, m_temperature);
    }
  }

private:
  int hexadecimalToDecimal(String hexVal) {
    int len = hexVal.length();
    int base = 1;

    int dec_val = 0;

    for (int i = len - 1; i >= 0; i--) {
      if (hexVal[i] >= '0' && hexVal[i] <= '9') {
        dec_val += (hexVal[i] - 48) * base;

        base = base * 16;
      } else if (hexVal[i] >= 'A' && hexVal[i] <= 'F') {
        dec_val += (hexVal[i] - 55) * base;

        base = base * 16;
      }
    }
    return dec_val;
  }

  void decodeRuuvi(String hex_data, int rssi) {
    if (hex_data.substring(4, 6) == "05") {
      m_temperature = hexadecimalToDecimal(hex_data.substring(6, 10)) * 0.005;
      m_humidity = hexadecimalToDecimal(hex_data.substring(10, 14)) * 0.0025;
      m_pressure = hexadecimalToDecimal(hex_data.substring(14, 18)) * 1 + 50000;

      m_ax = hexadecimalToDecimal(hex_data.substring(18, 22));
      m_ay = hexadecimalToDecimal(hex_data.substring(22, 26));
      m_az = hexadecimalToDecimal(hex_data.substring(26, 30));

      if (m_ax > 0xF000) {
        m_ax = m_ax - (1 << 16);
      }
      if (m_ay > 0xF000) {
        m_ay = m_ay - (1 << 16);
      }
      if (m_az > 0xF000) {
        m_az = m_az - (1 << 16);
      }

      m_voltage_power = hexadecimalToDecimal(hex_data.substring(30, 34));
      m_voltage = (int)((m_voltage_power & 0x0b1111111111100000) >> 5) + 1600;
      m_power = (int)(m_voltage_power & 0x0b0000000000011111) - 40;

      m_rssi_ruuvi = rssi;

      m_movement = hexadecimalToDecimal(hex_data.substring(34, 36));
      m_measurement = hexadecimalToDecimal(hex_data.substring(36, 40));
    } else {
      Serial.print("unexpected data: ");
      Serial.print(hex_data.substring(4, 6));
      Serial.println();
    }
  }

  String MAC_add = "f8:56:bb:f1:d5:99";

  int m_voltage_power;
  double m_temperature;
  double m_humidity;
  double m_pressure;
  double m_ax;
  double m_ay;
  double m_az;
  double m_voltage;
  double m_power;
  double m_rssi_ruuvi;
  double m_movement;
  double m_measurement;

  int m_i = 0;
  std::function<void(double, double)> m_draw_function;
};