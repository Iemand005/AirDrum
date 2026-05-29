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
    bool _stompConnected;
    bool _warnedNotConnected;

    static void _webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
        if (WebSocketClient::instance != nullptr) {
            WebSocketClient::instance->handleEvent(type, payload, length);
        }
    }

    void sendFrame(const String& frame) {
        // STOMP frames must be NULL-terminated, so we explicitly include the trailing '\0'.
        _webSocket.sendTXT(frame.c_str(), frame.length() + 1);
    }

    void handleEvent(WStype_t type, uint8_t * payload, size_t length) {
        switch(type) {
            case WStype_DISCONNECTED:
                Serial.println("[WS] Verbinding verbroken! Automatische herstelpoging loopt...");
                _wsConnected = false;
                _stompConnected = false;
                _warnedNotConnected = false;
                break;
                
            case WStype_CONNECTED:
                Serial.println("[WS] Ruwe WebSocket verbonden. STOMP handshake starten...");
                _wsConnected = true;
                {
                    String connectFrame = "CONNECT\naccept-version:1.1,1.2\nheart-beat:10000,10000\n\n";
                    sendFrame(connectFrame);
                }
                break;
                
            case WStype_TEXT: {
                String msg((char*)payload);
                if (msg.startsWith("CONNECTED")) {
                    Serial.println("[WS] STOMP verbinding succesvol tot stand gebracht!");
                    _stompConnected = true;
                    _warnedNotConnected = false;
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
        _wsConnected = false;
        _stompConnected = false;
        _warnedNotConnected = false;
        WebSocketClient::instance = this;
    }

    void begin() {
        _webSocket.onEvent(_webSocketEvent);
        _webSocket.begin(_host, _port, _path);
        _webSocket.setReconnectInterval(5000); 
    }

    void loop() {
        _webSocket.loop();
    }

    void sendText(String payload) {
        if (!_stompConnected) {
            if (!_warnedNotConnected) {
                Serial.println("[WS] Kan bericht niet verzenden: STOMP is niet verbonden!");
                _warnedNotConnected = true;
            }
            return;
        }

        String stompFrame = "SEND\n";
        stompFrame += "destination:/app/drum.send\n"; 
        stompFrame += "content-length:" + String(payload.length()) + "\n\n";
        stompFrame += payload;

        sendFrame(stompFrame);
    }

    bool isConnected() {
        return _stompConnected;
    }
};

inline WebSocketClient* WebSocketClient::instance = nullptr;

#endif
