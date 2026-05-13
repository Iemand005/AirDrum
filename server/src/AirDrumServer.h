
#ifndef AirDrumServer_h
#define AirDrumServer_h

#include <Arduino.h>
#include <WiFi.h>

class AirDrumServer {
  void disableWiFi() {
    WiFi.disconnect(true); 
    WiFi.mode(WIFI_OFF);
  }

  void disableBluetooth() {
    
  }
};

#endif