
#ifndef AIR_DRUM_DATA_H
#define AIR_DRUM_DATA_H

#include <stdint.h>

struct AirDrumData {
  uint8_t code;
  uint8_t key;
  uint8_t value;
};

union DataPacket {
  AirDrumData drum;
  uint32_t rawData;
};

#endif