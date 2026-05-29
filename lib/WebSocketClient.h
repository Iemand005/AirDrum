#ifndef WebSocketClient_h
#define WebSocketClient_h

#include <Arduino.h>
#include <WebSocketsClient.h>

class WebSocketClient {
private:
    WebSocketsClient _webSocket;
    const char* _host;
    int _port;
    const char* _path;
    bool _wsConnected;
    bool _warnedNotConnected;

    static void _webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
        if (WebSocketClient::instance != nullptr) {
            WebSocketClient::instance->handleEvent(type, payload, length);
        }
    }

    void handleEvent(WStype_t type, uint8_t * payload, size_t length) {
        switch(type) {
            case WStype_DISCONNECTED:
                Serial.println("[WS] Verbinding verbroken! Automatische herstelpoging loopt...");
                _wsConnected = false;
                _warnedNotConnected = false;
                break;
                
            case WStype_CONNECTED:
                Serial.println("[WS] WebSocket verbonden.");
                _wsConnected = true;
                break;
                
            case WStype_TEXT: {
                String msg((char*)payload);
                Serial.print("[WS] Ontvangen: ");
                Serial.println(msg);
                break;
            }
            default:
                break;
        }
    }

public:
    static WebSocketClient* instance;

    WebSocketClient(const char* host, int port, const char* path) {
        _host = host;
        _port = port;
        _path = path;
        _wsConnected = false;
        _warnedNotConnected = false;
        WebSocketClient::instance = this;
    }

    void begin() {
        _webSocket.onEvent(_webSocketEvent);
        _webSocket.setExtraHeaders();
        _webSocket.begin(_host, _port, _path);
        _webSocket.setReconnectInterval(5000); 
    }

    void loop() {
        _webSocket.loop();
    }

    void sendText(String payload) {
        if (!_wsConnected) {
            if (!_warnedNotConnected) {
                Serial.println("[WS] Kan bericht niet verzenden: WebSocket is niet verbonden!");
                _warnedNotConnected = true;
            }
            return;
        }

        _webSocket.sendTXT(payload);
    }

    bool isConnected() {
        return _wsConnected;
    }
};

inline WebSocketClient* WebSocketClient::instance = nullptr;

#endif
