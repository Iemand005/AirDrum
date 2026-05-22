
#ifndef AirDrumServer_h
#define AirDrumServer_h

#include <Arduino.h>
#include <WiFi.h>

#include "SocketServer.h"
// #inlcude 
#include "WhaddaServer.h"

class AirDrumServer : public WebSocketServer, public WhaddaServer {

public:

  AirDrumServer() : WebSocketServer() {

  }

  void disableWiFi() {
    WiFi.disconnect(true); 
    WiFi.mode(WIFI_OFF);
  }

  bool connectWiFi(const char *ssid, const char *password) {
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }

    Serial.println("");
    Serial.println("Wi-Fi Connected!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());

    return true;
  }

  void disableBluetooth() {
    
  }

  void broadcastAcceleration(float accel) {
    String msg = "ACCEL:" + String(accel, 2); 
    sendText(msg);
  }

private:
  void sendText(const String& message) {
    if (ws.count() > 0) {
      ws.textAll(message);
    }
  }
};

#endif