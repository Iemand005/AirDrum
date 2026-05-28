
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
    if (mySwitch.available()) {
        unsigned long receivedCode = mySwitch.getReceivedValue();

        DataPacket data;
        data.rawData = receivedCode;

        mySwitch.resetAvailable();

        return data.drum.value;
    }

    return -1;
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