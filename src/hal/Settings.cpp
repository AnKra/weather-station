#include "hal/Settings.h"

#include "SPIFFS.h"

namespace hal {

void readline(File& f, std::string& target) {
  // Buffer long enough to hold an SSID (32 characters) or a WPA2 PSK (up to 63
  // ASCII or 64 HEX) and a terminating null byte
  constexpr static const size_t bufferSize = 64 + 1;
  char buffer[bufferSize];

  target.erase();

  memset(&buffer, 0, bufferSize);
  size_t bytesRead = f.readBytesUntil('\n', reinterpret_cast<uint8_t*>(&buffer),
                                      bufferSize - 1);
  if (bytesRead > 0) {
    target.append(buffer);
  }
}

void Settings::load() {
  File f = SPIFFS.open(kWifiSettingsFilename_);

  if (!f) {
    printf("Opening '%s' for reading failed.\n", kWifiSettingsFilename_);
  } else {
    readline(f, ssid_);
    readline(f, password_);
    f.close();
  }
}

void Settings::print() {
  Serial.print("SSID: ");
  Serial.print(ssid_.c_str());
  Serial.print(" Password: ");
  Serial.print(password_.c_str());
}

}  // namespace hal
