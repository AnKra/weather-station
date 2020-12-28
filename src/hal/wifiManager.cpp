#include "wifiManager.h"

namespace hal {

// Module-Local variables
boolean wifiConnected = false;

constexpr static const char* ntpServer = "pool.ntp.org";

// Module-Local functions
void wifiEventHandler(WiFiEvent_t event);

void startNtp();

// Module implementation

void wifiEventHandler(WiFiEvent_t event) {
  switch (event) {
    case SYSTEM_EVENT_AP_STADISCONNECTED:
    case SYSTEM_EVENT_STA_LOST_IP:
      wifiConnected = false;
      Serial.println("Wifi disconnected.");
      break;
    case SYSTEM_EVENT_ETH_GOT_IP:
    case SYSTEM_EVENT_STA_GOT_IP: {
      IPAddress localIP = WiFi.localIP();
      IPAddress subnetMask = WiFi.subnetMask();

      Serial.print("Wifi connected. IP: ");
      localIP.printTo(Serial);
      Serial.print("/");
      subnetMask.printTo(Serial);

      uint32_t subnetBits = subnetMask;
      // Invert the subnetBits
      subnetBits = ~subnetBits;
      // Set the inverted bits in the IP address
      uint32_t broadcastBits = localIP;
      broadcastBits |= subnetBits;
      IPAddress broadcastAddress{broadcastBits};

      Serial.print("/");
      broadcastAddress.printTo(Serial);

      Serial.println();

      wifiConnected = true;
      startNtp();
      break;
    }
    case SYSTEM_EVENT_WIFI_READY:
      Serial.println("Wifi on");
      break;
    case SYSTEM_EVENT_STA_START:
      Serial.println("Wifi: Starting STA mode");
      break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
      Serial.println("Wifi: STA Disconnected. Connection failed.");
      wifiConnected = false;
      break;
    default:
      Serial.print("Wifi event: ");
      Serial.println(event);
      // Don't care about other events
      break;
  }
}

void setupWifi() {
  WiFi.disconnect(false);
  WiFi.onEvent(wifiEventHandler);
}

void startWifi(const char* ssid, const char* passphrase) {
  Serial.println("Starting Wifi. Waiting for connection.");
  WiFi.begin(ssid, passphrase);
  WiFi.setAutoReconnect(true);
}

void stopWifi() {
  Serial.println("Stopping Wifi.");
  WiFi.setAutoReconnect(false);
  WiFi.disconnect(false);
}

bool isWifiAvailable() { return wifiConnected; }

void startNtp() {
  const long gmtOffset_sec = 3600;  // GMT + 1 hour
  int daylightOffset_sec = 0;       // Ignore daylight savings time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}

}  // namespace hal
