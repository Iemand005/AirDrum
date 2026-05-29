
#ifndef AirDrumServer_h
#define AirDrumServer_h

#include <Arduino.h>
#include <WiFi.h>
#include <SerialCodes.h>

#include "WebSocketClient.h"
#include "VecMath.h"
#include "WhaddaServer.h"
#include "WhaddaClient.h"

const String SERVER_ADDRESS = "10.25.238.47";

class AirDrumClient : public WebSocketClient, public WhaddaServer, public WhaddaClient {

public:

  AirDrumClient() : WebSocketClient(SERVER_ADDRESS, 8080, "/ws/websocket") {

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

    #ifdef WHADDA_TRANSMITTER
    sendKeyValue(69, drumId);
    #endif
  }
};

#endif