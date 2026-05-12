
#include <Arduino.h>


/**
 * @brief Vector of 3 16bit integers
 */
struct Vec3I16 {
    int16_t x, y, z;

    Vec3I16 operator-(const Vec3I16& other) const {
        return {x - other.x, y - other.y, z - other.z};
    }
};

/**
 * @brief Vector of 3 floats
 */
struct Vec3 {
    float x, y, z;

    void normalize() {
        // float length = std::sqrt(x * x + y * y + z * z);
        // if (length > 0.0f) {
        //     x /= length;
        //     y /= length;
        //     z /= length;
        // }
    }
};