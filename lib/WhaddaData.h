
#ifndef WhaddaData_h
#define WhaddaData_h

#include <stdint.h>

struct WhaddaData {
  uint8_t code;
  uint8_t key;
  uint8_t value;
};

union DataPacket {
  WhaddaData drum;
  uint32_t rawData;
};

#endif