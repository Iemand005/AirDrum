
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
      // unsigned long combined = (code << 8) | value;
      WhaddaData data;

      data.code = code;
      data.key = 0;
      data.value = value;

      this->mySwitch.send(data.rawData, 24);
      code++;
  }
};

#endif