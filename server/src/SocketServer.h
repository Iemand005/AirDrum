#ifndef SocketServer_h
#define SocketServer_h

#include <ESPAsyncWebServer.h>



class SocketServer {

  AsyncWebServer server;
  AsyncWebSocket ws;

  void startListener() {
    ws.onEvent([](AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len) {

    });
    server.addHandler(&ws);
    
    server.begin();
  }
};

#endif