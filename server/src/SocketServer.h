#ifndef SocketServer_h
#define SocketServer_h

#include <ESPAsyncWebServer.h>



class SocketServer {

  AsyncWebServer server;
  AsyncWebSocket ws;

public:
  SocketServer() : server(80), ws("/ws") {}
  void startListener() {
    ws.onEvent([this](AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len) {
      if (type == WS_EVT_DATA) {
        data[len] = 0;
        String message = (char*)data;
        
        Serial.print("WebSocket message! ");
        Serial.println(message);
        // 8
      }
    });
    server.addHandler(&ws);
    
    server.begin();
  }
};

#endif