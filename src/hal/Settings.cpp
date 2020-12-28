#include "hal/Settings.h"

#include <SPIFFS.h>

namespace hal {

void readline(File& file, std::string& target) {
  // Buffer long enough to hold an SSID (32 characters) or a WPA2 PSK (up to 63
  // ASCII or 64 HEX) and a terminating null byte
  constexpr static const size_t bufferSize = 64 + 1;
  char buffer[bufferSize];

  target.erase();

  memset(&buffer, 0, bufferSize);
  size_t bytesRead = file.readBytesUntil('\n', reinterpret_cast<uint8_t*>(&buffer), bufferSize - 1);
  if (bytesRead > 0) {
    target.append(buffer);
  }
}

void Settings::load() {
  File file = SPIFFS.open(kWifiSettingsFilename_);

  if (!file) {
    printf("Opening '%s' for reading failed.\n", kWifiSettingsFilename_);
    return;
  }

  readline(file, ssid_);
  readline(file, password_);
  file.close();
}

void Settings::print() {
  Serial.print("SSID: ");
  Serial.print(ssid_.c_str());
  Serial.print(" Password: ");
  Serial.print(password_.c_str());
}

}  // namespace hal
