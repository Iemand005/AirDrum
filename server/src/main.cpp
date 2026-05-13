#include <Arduino.h>
#include <RCSwitch.h>
#include <Wire.h>
// #include <Math.h>
#include <AirDrumServer.h>
#include <WiFiCredentials.h>


// Accelerometer reader

/*
MPU-9265  Arduino Nano
VCC       3.3V
GND       GND
SDA       A4
SCL       A5
*/

/*
WPI469T   Arduino Nano
VCC       3.3V
GND       GND
SIG       D2
*/

/*
LED     Arduino
VCC -> resistor 220 Ohm -> Arduino D3
*/

#define pinButton 5
#define pinLed 2 // Set to 3 for arduino
#define pinTransmitter 4


#define transmitRepeat 3

// MPU-9265 I2C address
#define MPU_ADDR 0x68

// MPU-9265 registers (See MPU-9250-Register-Map documentation page 8)
#define REG_PWR_MGMT_1 0x6B
#define REG_ACCEL_CONFIG 0x1C
#define REG_ACCEL_XOUT_H 0x3B
#define GYRO_XOUT_H 0x43
#define REG_WHO_AM_I 0x75

// Threshold for movement detection (tune as needed)
const int threshold = 1;

const int gyroXdiff = 20;
const int gyroXThreshold = 1500;
const int gyroXIgnoreBelow = 20;
const int gyroXCancel = -5;
int gyroXSum = 0;
bool hasHit = true;

bool moving = false;
const int stopMovingThreshold = 3;
int moveTriggerCount = 0;

const bool applyLowPassFilter = true;

#define USE_WIFI

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


// Three-axis baseline values for resting position
Vec3I16 baseAcceleration;
Vec3 gyroBase, gyroLast;
Vec3I16 lastAccel{0,0,0};



// Transmitter variables

RCSwitch mySwitch = RCSwitch();

int code = 0;
bool isButtonPressed = false;


AirDrumServer server;


// Low pass filter for accelerometer data to take out the garvity acceleration

Vec3 filterState = { 0.0f, 0.0f, 16384.0f };

// Filter coefficient
const float ALPHA = 0.15f; 

Vec3I16 lowPassFilter(Vec3I16 raw) {
    filterState.x = (raw.x * ALPHA) + (filterState.x * (1.0f - ALPHA));
    filterState.y = (raw.y * ALPHA) + (filterState.y * (1.0f - ALPHA));
    filterState.z = (raw.z * ALPHA) + (filterState.z * (1.0f - ALPHA));

    Vec3I16 smoothed;
    smoothed.x = (int16_t)filterState.x;
    smoothed.y = (int16_t)filterState.y;
    smoothed.z = (int16_t)filterState.z;

    return smoothed;
}

/**
 * @brief Write a byte to a specific register on the MPU-9265
 * 
 * @param reg 
 * @param value 
 */
void writeRegister(uint8_t reg, uint8_t value) {
    Wire.beginTransmission(MPU_ADDR);
    Wire.write(reg);
    Wire.write(value);
    Wire.endTransmission();
}

/**
 * @brief Read accelerometer data from the MPU-9265 and return it as three 16-bit integers
 * 
 * @returns Vector of acceleration
 */
Vec3I16 readAccel() {
    Wire.beginTransmission(MPU_ADDR);
    Wire.write(REG_ACCEL_XOUT_H);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU_ADDR, 6);

    // MPU-9265 data is big-endian (high byte first)
    Vec3I16 acceleration;
    acceleration.x = (Wire.read() << 8) | Wire.read();
    acceleration.y = (Wire.read() << 8) | Wire.read();
    acceleration.z = (Wire.read() << 8) | Wire.read();

    if (applyLowPassFilter) acceleration = lowPassFilter(acceleration);

    return acceleration;
}

Vec3I16 readLinearAccel() {
    return readAccel() - baseAcceleration;
}


/**
 * @brief Read gyroscope data from the MPU-9265 and return it as three floats of degrees per second
 * 
 * @returns Vector of acceleration
 */
Vec3 readGyro() {
    Wire.beginTransmission(MPU_ADDR);
    Wire.write(GYRO_XOUT_H);
    Wire.endTransmission(false);
    
    // Request 6 bytes (2 bytes for each axis: X, Y, Z)
    Wire.requestFrom(MPU_ADDR, 6, true);
    
    Vec3I16 gyroRaw;
    gyroRaw.x = (Wire.read() << 8) | Wire.read(); 
    gyroRaw.y = (Wire.read() << 8) | Wire.read(); 
    gyroRaw.z = (Wire.read() << 8) | Wire.read(); 

    // Convert to degrees per second (dps)
    Vec3 dps;
    dps.x = gyroRaw.x / 131.0;
    dps.y = gyroRaw.y / 131.0;
    dps.z = gyroRaw.z / 131.0;

    return dps;
}


// Transmitter code


void setup() {

    // Accelerometer init

    Serial.begin(115200);
    Serial.println("Starting MPU-9265 accelerometer...");

    pinMode(pinLed, OUTPUT);
    pinMode(pinButton, INPUT_PULLUP);

    Wire.begin();

    // Wake up MPU-9265 (clear sleep bit)
    writeRegister(REG_PWR_MGMT_1, 0x00);
    // Set accelerometer range to +/-2g
    writeRegister(REG_ACCEL_CONFIG, 0x00);

    delay(100);

    // Read baseline (resting position)
    auto acceleration = readAccel();
    gyroBase = readGyro();

    baseAcceleration = acceleration;
    Serial.print("Baseline - X: "); Serial.print(baseAcceleration.x);
    Serial.print(" Y: "); Serial.print(baseAcceleration.y);
    Serial.print(" Z: "); Serial.println(baseAcceleration.z);

    Serial.println("Setup complete");

    // Server init

    // Serial.begin(115200);
    Serial.println("Starting Wadda server...");

    pinMode(pinLed, OUTPUT);

    mySwitch.enableTransmit(pinTransmitter);

    mySwitch.setRepeatTransmit(transmitRepeat);


    // Wifi stuffs
#ifdef USE_WIFI
    server.connectWiFi(SECRET_SSID, SECRET_PASS);

    server.startListener();
#endif

    Serial.println("Setup complete");
}

void loop() {

    // Reaad accel data
    auto currentAccel = readLinearAccel();
    auto lowPassedAccel = lowPassFilter(currentAccel);
    

    auto jerk = currentAccel - lastAccel;
    lastAccel = currentAccel;

    int magnitude = jerk.magnitude();

    magnitude -= 100;
    magnitude /= 10;
    
    if (magnitude > threshold) {
        moving = true;
        moveTriggerCount++;
        digitalWrite(pinLed, HIGH);
        // analogWrite(pinLed, 10);
        Serial.print("Movement detected! Magnitude: ");
        Serial.println(magnitude);

        auto rotation = readGyro();

        if (rotation.x < 0) hasHit = false;

        if (abs(rotation.x) > gyroXIgnoreBelow) {

            Serial.print("Gyro X Sum: "); Serial.println(gyroXSum);

            // if (rotation.x > )
            if (rotation.x < gyroXCancel && gyroXSum > 500) {
                gyroXSum = 0;
                
                int value = 67;
                unsigned long combined = (code << 8) | value;
                mySwitch.send(combined, 24);
                code++;

            }

            gyroXSum += rotation.x;
            if (gyroXSum < 0) gyroXSum = 0;

            if (!hasHit && gyroXSum > gyroXThreshold) {
                // if () 
                hasHit = true;
                Serial.println("MEIW!");

                int value = 69;

                // int combined = (code * 1000) + 69;
                unsigned long combined = (code << 8) | value;


                mySwitch.send(combined, 24);
                code++;
                gyroXSum = 0;
            }
        } else {
            Serial.println("Didn't rotate enough. Ignoring...");
        }

        // code++;
        // mySwitch.send(code, 24);
    } else {
        digitalWrite(pinLed, LOW);
    }

    // Broadcast


    bool digitalReadButton = !digitalRead(pinButton);

    if (digitalReadButton == HIGH && !isButtonPressed) {
        isButtonPressed = true;

        Serial.println("Button pressed");
        digitalWrite(pinLed, HIGH);

        code++;
        mySwitch.send(code, 24);


        
    } else if (digitalReadButton == LOW && isButtonPressed) {
        isButtonPressed = false;

        Serial.println("Button released");
        digitalWrite(pinLed, LOW);
    }

    delay(1);
}