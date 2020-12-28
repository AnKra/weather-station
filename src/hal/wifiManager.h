#ifndef __HAL_WIFIMANAGER_H__
#define __HAL_WIFIMANAGER_H__

#include <WiFi.h>

#include <WiFiClient.h>

namespace hal {

void setupWifi();
void startWifi(const char* ssid, const char* passphrase);
void stopWifi();

bool isWifiAvailable();

}  // namespace hal

#endif  // __HAL_WIFIMANAGER_H__