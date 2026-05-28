
#ifndef WhaddaServer_h
#define WhaddaServer_h

#include <RCSwitch.h>

#include <WhaddaData.h>

class WhaddaServer {
  int code = 0;
  RCSwitch mySwitch;

public:

  WhaddaServer() : mySwitch() {}

  void sendValue(int value) {
    sendKeyValue(0, value);
  }
  
  void sendKeyValue(int key, int value) {
      // unsigned long combined = (code << 8) | value;
      WhaddaData data;

      data.code = code;
      data.key = key;
      data.value = value;

      DataPacket packet;
      packet.drum = data;

      this->mySwitch.send(packet.rawData, 24);
      code++;
  }
};

#endif