#ifndef SocketServer_h
#define SocketServer_h

#include <ESPAsyncWebServer.h>



class SocketServer {

  AsyncWebServer server(80);
  AsyncWebSocket ws("/socket");

};

#endif