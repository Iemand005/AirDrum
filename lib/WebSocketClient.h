#ifndef WebSocketClient_h
#define WebSocketClient_h

#include <Arduino.h>
#include <WebSocketsClient.h>

class WebSocketClient {
private:
    static constexpr size_t MAX_PENDING_MESSAGES = 8;
    WebSocketsClient _webSocket;
    const char* _host;
    int _port;
    const char* _path;
    bool _wsConnected;
    size_t _pendingHead;
    size_t _pendingTail;
    String _pendingMessages[MAX_PENDING_MESSAGES];

    static void _webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
        if (WebSocketClient::instance != nullptr)
            WebSocketClient::instance->handleEvent(type, payload, length);
    }

    void enqueueMessage(const String& payload) {
        size_t nextTail = (_pendingTail + 1) % MAX_PENDING_MESSAGES;
        if (nextTail == _pendingHead)
            _pendingHead = (_pendingHead + 1) % MAX_PENDING_MESSAGES;

        _pendingMessages[_pendingTail] = payload;
        _pendingTail = nextTail;
    }

    void flushPendingMessages() {
        while (_pendingHead != _pendingTail && _wsConnected) {
            _webSocket.sendTXT(_pendingMessages[_pendingHead]);
            _pendingMessages[_pendingHead] = "";
            _pendingHead = (_pendingHead + 1) % MAX_PENDING_MESSAGES;
        }
    }

    void handleEvent(WStype_t type, uint8_t * payload, size_t length) {
        switch(type) {
            case WStype_DISCONNECTED:
                Serial.println("[WS] Verbinding verbroken! Automatische herstelpoging loopt...");
                _wsConnected = false;
                break;
                
            case WStype_CONNECTED:
                Serial.println("[WS] WebSocket verbonden.");
                _wsConnected = true;
                flushPendingMessages();
                break;
                
            case WStype_TEXT: {
                String msg((char*)payload);
                Serial.print("[WS] Ontvangen: ");
                Serial.println(msg);
                break;
            }
        }
    }

public:
    static WebSocketClient* instance;

    WebSocketClient(const char* host, int port, const char* path) {
        _host = host;
        _port = port;
        _path = path;
        _wsConnected = false;
        _pendingHead = 0;
        _pendingTail = 0;
        WebSocketClient::instance = this;
    }

    void begin() {
        _webSocket.onEvent(_webSocketEvent);
        _webSocket.setExtraHeaders();
        _webSocket.begin(_host, _port, _path);
        _webSocket.setReconnectInterval(1000); 
    }

    void loop() {
        _webSocket.loop();
    }

    void sendText(String payload) {
        if (!_wsConnected) {
            enqueueMessage(payload);
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
