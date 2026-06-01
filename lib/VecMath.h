
#ifndef Math_h
#define Math_h

#include <stdint.h>


/**
 * @brief Vector of 3 16bit integers
 */
struct Vec3I16 {
    int16_t x, y, z;

    Vec3I16 operator-(const Vec3I16& other) const {
        return {x - other.x, y - other.y, z - other.z};
    }

    int magnitude() {
        return abs(x) + abs(y) + abs(z);
    }
};

/**
 * @brief Vector of 3 floats
 */
struct Vec3 {
    float x, y, z;
};

#endif