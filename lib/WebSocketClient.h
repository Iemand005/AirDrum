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
    bool _stompConnected;

    // Statische wrapper voor de bibliotheek callback
    static void _webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
        if (WebSocketClient::instance != nullptr) {
            WebSocketClient::instance->handleEvent(type, payload, length);
        }
    }

    void handleEvent(WStype_t type, uint8_t * payload, size_t length) {
        switch(type) {
            case WStype_DISCONNECTED:
                Serial.println("[WS] Verbinding verbroken! Automatische herstelpoging loopt...");
                _stompConnected = false;
                break;
                
            case WStype_CONNECTED:
                Serial.println("[WS] Ruwe WebSocket verbonden. STOMP handshake starten...");
                _webSocket.sendTXT("CONNECT\naccept-version:1.1,1.2\nheart-beat:10000,10000\n\n\0");
                break;
                
            case WStype_TEXT: {
                String msg = String((char*)payload);
                if (msg.startsWith("CONNECTED")) {
                    Serial.println("[WS] STOMP verbinding succesvol tot stand gebracht!");
                    _stompConnected = true;
                }
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
        _stompConnected = false;
        WebSocketClient::instance = this;
    }

    void begin() {
        _webSocket.begin(_host, _port, _path);
        _webSocket.onEvent(_webSocketEvent);
        _webSocket.setReconnectInterval(5000); 
    }

    void loop() {
        _webSocket.loop();
    }

    void sendText(String payload) {
        if (!_stompConnected) {
            Serial.println("[WS] Kan bericht niet verzenden: STOMP is niet verbonden!");
            return;
        }

        String stompFrame = "SEND\n";
        stompFrame += "destination:/app/drum.send\n"; 
        stompFrame += "content-length:" + String(payload.length()) + "\n\n";
        stompFrame += payload;
        stompFrame += "\0";

        _webSocket.sendTXT(stompFrame);
    }

    // Status check
    bool isConnected() {
        return _stompConnected;
    }
};

inline WebSocketClient* WebSocketClient::instance = nullptr;

#endif
