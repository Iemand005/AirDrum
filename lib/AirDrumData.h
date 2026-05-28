
#ifndef AIR_DRUM_DATA_H
#define AIR_DRUM_DATA_H

#include <stdint.h>

struct AirDrumData {
  uint8_t sensorId;
  uint8_t velocity;
  uint8_t drumType;
};

union DataPacket {
  AirDrumData drum;
  uint32_t rawData;
};

#endif