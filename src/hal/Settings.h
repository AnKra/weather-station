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

 private:
  std::string ssid_;
  std::string password_;
};

}  // namespace hal

#endif  // __HAL__SETTINGS_H__
