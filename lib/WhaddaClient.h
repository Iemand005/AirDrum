
#ifndef WhaddaClient_h
#define WhaddaClient_h

#include <RCSwitch.h>

#include <WhaddaData.h>

class WhaddaClient {
  int code = 0;
  RCSwitch mySwitch;

public:

  WhaddaClient() : mySwitch() {}

  int receiveValue() {
    return receiveData().value;
  }

  WhaddaData receiveData() {
    WhaddaData result = { 0, 0, 0 };

    if (mySwitch.available()) {
        unsigned long receivedCode = mySwitch.getReceivedValue();

        DataPacket data;
        data.rawData = receivedCode;

        mySwitch.resetAvailable();

        result = data.drum;
    }

    return result;
  }
};

#endif