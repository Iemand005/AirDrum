#ifndef SocketServer_h
#define SocketServer_h

#include <ESPAsyncWebServer.h>

const int RED_LED_PIN = 4; 

class WebSocketServer {

  AsyncWebServer server;
  AsyncWebSocket ws;

public:
  WebSocketServer() : server(80), ws("/ws") {}
  void startListener() {
    ws.onEvent([this](AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len) {
      if (type == WS_EVT_DATA) {
        data[len] = 0;
        String message = (char*)data;
        
        Serial.print("WebSocket message! ");
        Serial.println(message);
        // 8
      }
      if (type == WS_EVT_CONNECT) {
        Serial.println("WebSocket client connected");
        digitalWrite(RED_LED_PIN, HIGH); 
      }
      if (type == WS_EVT_DISCONNECT) {
        Serial.println("WebSocket client disconnected");
        digitalWrite(RED_LED_PIN, LOW); 
      }
    });
    server.addHandler(&ws);
    
    server.begin();
  }

  void sendText(const String& message) {
    if (ws.count() > 0) {
      ws.textAll(message);
    }
  }
};

#endif