
#ifndef WhaddaClient_h
#define WhaddaClient_h

#include <RCSwitch.h>

class WhaddaServer {
  int code = 0;
  RCSwitch mySwitch;

public:

  WhaddaServer() : mySwitch() {}

  void sendValue(int value) {
      unsigned long combined = (code << 8) | value;

      this->mySwitch.send(combined, 24);
      code++;
  }
};

#endif