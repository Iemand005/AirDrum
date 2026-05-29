#ifndef SocketServer_h
#define SocketServer_h

#include <ESPAsyncWebServer.h>


class WebSocketServer {

  AsyncWebServer server;
  AsyncWebSocket ws;
  unsigned long lastPingTime = 0;
  const unsigned long PING_INTERVAL = 30000;

public:
  WebSocketServer() : server(80), ws("/ws") {}
  void startListener() {
    ws.onEvent([this](AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len) {
      if (type == WS_EVT_CONNECT) {
        Serial.println("WebSocket client connected");
        digitalWrite(RED_LED_PIN, HIGH);
      } 
      else if (type == WS_EVT_DISCONNECT) {
        Serial.println("WebSocket client disconnected");
        digitalWrite(RED_LED_PIN, LOW);
      } 
      else if (type == WS_EVT_DATA) {
        if (len > 0) {
          uint8_t *dataBuf = new uint8_t[len + 1];
          memcpy(dataBuf, data, len);
          dataBuf[len] = 0;
          String message = (char*)dataBuf;
          
          Serial.print("WebSocket message! ");
          Serial.println(message);
          
          delete[] dataBuf;
        }
      }
      else if (type == WS_EVT_PONG || type == WS_EVT_PING) {
        Serial.println("WebSocket ping/pong");
      }
    });
    
    // Configure websocket settings
    // ws.setAuthentication("user", "pass");  // Optional: add basic auth
    server.addHandler(&ws);
    
    ws.enable(true);
    
    server.begin();
  }

  void sendText(const String& message) {
    if (ws.count() > 0) {
      ws.textAll(message);
    }
  }

  void keepAlive() {
    unsigned long now = millis();
    if (now - lastPingTime >= PING_INTERVAL) {
      lastPingTime = now;
      ws.pingAll();
    }
  }
};

#endif