
#ifndef AirDrumServer_h
#define AirDrumServer_h

#include <Arduino.h>
#include <WiFi.h>

class AirDrumServer {
  void disableWiFi() {
    WiFi.disconnect(true); 
    WiFi.mode(WIFI_OFF);
  }

  bool wifiConnect(const char *ssid, const char *password) {
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }

    Serial.println("");
    Serial.println("Wi-Fi Connected!");
    Serial.print("IP-adres of: ");
    Serial.println(WiFi.localIP());
  }

  void disableBluetooth() {
    
  }
};

#endif