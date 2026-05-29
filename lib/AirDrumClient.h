
#ifndef AirDrumServer_h
#define AirDrumServer_h

#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <SerialCodes.h>

#include "WebSocketClient.h"
#include "VecMath.h"
#include "WhaddaServer.h"
#include "WhaddaClient.h"

const char* SERVER_ADDRESS = "10.25.238.47";

class AirDrumClient : public WebSocketClient, public WhaddaServer, public WhaddaClient {

public:

  AirDrumClient() : WebSocketClient(SERVER_ADDRESS, 8080, "/ws/raw") {

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
    DynamicJsonDocument doc(192);
    doc["deviceId"] = "esp32";
    doc["eventType"] = "ACCEL";
    doc["timestamp"] = millis();
    JsonObject payload = doc.createNestedObject("payload");
    payload["x"] = accel.x;
    payload["y"] = accel.y;
    payload["z"] = accel.z;

    String json;
    serializeJson(doc, json);
    sendText(json);
  }

  void broadcastGyro(Vec3 gyroe) {
    DynamicJsonDocument doc(192);
    doc["deviceId"] = "esp32";
    doc["eventType"] = "GYRO";
    doc["timestamp"] = millis();
    JsonObject payload = doc.createNestedObject("payload");
    payload["x"] = gyroe.x;
    payload["y"] = gyroe.y;
    payload["z"] = gyroe.z;

    String json;
    serializeJson(doc, json);
    sendText(json);
  }

  void broadcastDrumHit(int drumId) {
    DynamicJsonDocument doc(128);
    doc["deviceId"] = "esp32";
    doc["eventType"] = "DRUM_HIT";
    doc["timestamp"] = millis();
    JsonObject payload = doc.createNestedObject("payload");
    payload["drumId"] = drumId;

    String json;
    serializeJson(doc, json);
    sendText(json);

    #ifdef WHADDA_TRANSMITTER
    sendKeyValue(69, drumId);
    #endif
  }
};

#endif
