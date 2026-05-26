
#ifndef AirDrumServer_h
#define AirDrumServer_h

#include <Arduino.h>
#include <WiFi.h>

#include "SocketServer.h"
#include "VecMath.h"
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
    printIp();

    return true;
  }

  void printIp() {
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
  }

  void disableBluetooth() {
    
  }

  void broadcastAcceleration(Vec3I16 accel) {
    String msg = "ACCEL:" + String((int)accel.x) + "," + String((int)accel.y) + "," + String((int)accel.z);
    sendText(msg);
  }

  void broadcastGyro(Vec3 gyroe) {
    String msg = "GYRO:" + String((int)gyroe.x) + "," + String((int)gyroe.y) + "," + String((int)(gyroe.z));
    sendText(msg);
  }

  void broadcastDrumHit(int drumId) {
    String msg = "DRUM_HIT:" + String(drumId);
    sendText(msg);
  }
};

#endif