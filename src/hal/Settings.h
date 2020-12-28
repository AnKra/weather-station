#ifndef __HAL__SETTINGS_H__
#define __HAL__SETTINGS_H__

#include <string>

namespace hal {

/*
 * \brief Class Settings
 */
class Settings {
 public:
  constexpr static const char* kWifiSettingsFilename_ = "/WifiConfig.ini";

  void load();
  void print();

  const std::string& getSsid() const { return ssid_; }
  const std::string& getPassword() const { return password_; }

 private:
  std::string ssid_;
  std::string password_;
};

}  // namespace hal

#endif  // __HAL__SETTINGS_H__
