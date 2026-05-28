#include <Arduino.h>
#include <RCSwitch.h>
#include <Wire.h>
// #include <Math.h>
#include <AirDrumServer.h>
#include <WiFiCredentials.h>
// #include "../lib/WhaddaServer.h"
#ifdef WHADDA_TRANSMITTER
#include <WhaddaServer.h>
#endif
#ifdef WHADDA_RECEIVER
#include <WhaddaClient.h>
#endif
#include <SerialCodes.h>
#include <VecMath.h>

#include <MPU9250_WE.h>

#define USE_WIFI
// #define USE_WHADDA
#define USE_WOM // Wake on mitionion

// Accelerometer reader

/*
MPU-9265  Arduino Nano
VCC       3.3V
GND       GND
SDA       A4
SCL       A5
*/

/*
MPU-9265  ESP32 
VCC       3.3V
GND       GND
SDA       GPIO 21
SCL       GPIO 22
INT       GPIO 12 (optional, for interrupts)
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

#define sleepThreshold 1000

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
const Vec3I16 gyroThreshold = {1500, 1500, 1500};
const int gyroXIgnoreBelow = 20;
const Vec3I16 gyroCancel = {-5, -5, -5};
Vec3I16 gyroSum = {0, 0, 0};
bool hasHit = true;

bool moving = false;
const int stopMovingThreshold = 3;
int stillMomentCount = 0;

const bool applyLowPassFilter = true;




// Three-axis baseline values for resting position
Vec3I16 baseAcceleration;
Vec3 gyroBase, gyroLast;
Vec3I16 lastAccel{0,0,0};



// Transmitter variables

RCSwitch mySwitch = RCSwitch();

int code = 0;

void sendValue(int value) {
    sendCodeOverSerial(code, value);
    
    unsigned long combined = (code << 8) | value;

    mySwitch.send(combined, 24);
    code++;
}

void sendKeyValue(int key, int value) {
    sendCodeOverSerial(code, value);
    
    unsigned long combined = (code << 8) | value;


    mySwitch.send(combined, 24);
    code++;
}

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



void enterDeepSleep() {
    esp_deep_sleep_start();
}

void airSetup() {

    
MPU9250_WE myMPU = MPU9250_WE(MPU_ADDR);

if(!myMPU.init()){
    Serial.println("MPU-9265 niet verbonden!");
  }

    myMPU.enableWakeOnMotion(MPU9250_WOM_ENABLE, MPU9250_WOM_COMP_ENABLE);
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_4, 1);
  

    // Accelerometer init

    Serial.begin(115200);
    Serial.println("Starting MPU-9265 accelerometer...");

    pinMode(pinLed, OUTPUT);
    pinMode(pinButton, INPUT_PULLUP);

    pinMode(RED_LED_PIN, OUTPUT);
    digitalWrite(RED_LED_PIN, LOW);

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

#ifdef USE_WHADDA
    Serial.println("Starting Whadda server...");

    pinMode(pinLed, OUTPUT);

    mySwitch.enableTransmit(pinTransmitter);

    mySwitch.setRepeatTransmit(transmitRepeat);
#endif

    // Wifi stuffs
#ifdef USE_WIFI
    server.connectWiFi(SECRET_SSID, SECRET_PASS);

    server.startListener();
#endif

    Serial.println("Setup complete");
}

unsigned long lastIpPrintTime = 0;
const unsigned long ipPrintInterval = 1000;

const bool broadcastAccel = true;

const bool verboseLog = false;


int instrumentId = 0;

void airLoop() {

    // Reaad accel data
    auto currentAccel = readLinearAccel();
    auto lowPassedAccel = lowPassFilter(currentAccel);
    
    server.keepAlive();

    auto jerk = currentAccel - lastAccel;
    lastAccel = currentAccel;

    int magnitude = jerk.magnitude();

    if (broadcastAccel) {
        auto currentTime = millis();
        if (currentTime - lastIpPrintTime >= ipPrintInterval) {
            lastIpPrintTime = currentTime;
            
            server.printIp();
        }

        server.broadcastAcceleration(currentAccel);
    }


//     server.receiveData();
// #endif

#ifdef WHADDA_RECEIVER
    auto data = server.receiveData();
    if (data.value.code != 0) {
        Serial.print("Received Whadda data - Code: ");
        Serial.print(data.code);
        Serial.print(" Key: ");
        Serial.print(data.key);
        Serial.print(" Value: ");
        Serial.println(data.value);
        server.broadcastDrumHit(data.value);
    }
    // server.

    #endif

    magnitude -= 100;
    magnitude /= 10;
    
    if (magnitude > threshold) {

        if (!moving && verboseLog) {
            Serial.println("I started moving! Wake up the WiFi or something if I turned it off over laying down for a minute");
        }

        moving = true;
        
        if (verboseLog) {
        Serial.print("Movement detected! Magnitude: ");
        Serial.println(magnitude);
        }

        auto rotation = readGyro();

        if (rotation.x < 0) hasHit = false;

        if (abs(rotation.x) > gyroXIgnoreBelow || abs(rotation.z) > gyroXIgnoreBelow) {

            Serial.print("Gyro X Sum: "); Serial.println(gyroSum.x);

            // if (rotation.x > )
            if (rotation.x < gyroCancel.x && gyroSum.x > 500) {
                gyroSum.x = 0;
                
                sendValue(67);
            }

            if (rotation.z < gyroCancel.z && gyroSum.z > 500) {
                gyroSum.z = 0;
                
                // sendValue(67);
            }

            gyroSum.x += rotation.x;
            gyroSum.z += rotation.z;
            if (gyroSum.x < 0) gyroSum.x = 0;

            if (!hasHit && gyroSum.x > gyroThreshold.x) {
                hasHit = true;
                Serial.println("MEIW!");

                server.broadcastDrumHit(instrumentId);

                // sendValue(69);

                gyroSum.x = 0;
            }


            if (gyroSum.z > gyroThreshold.z) {
                Serial.println("Switching drum!");

                sendValue(69);

                instrumentId = (instrumentId + 1) % 3;

                gyroSum.z = 0;
            }

            if (gyroSum.z < -gyroThreshold.z) {
                Serial.println("Switching drum!");

                instrumentId = (instrumentId - 1 + 3) % 3;

                sendValue(69);

                gyroSum.z = 0;
            }
        } else if (verboseLog) {
            Serial.println("Didn't rotate enough. Ignoring...");
        }

    } else {
        stillMomentCount++;
        moving = false;
        if (stillMomentCount > stopMovingThreshold) {
            hasHit = false;
            gyroSum.x = 0;
            gyroSum.z = 0;
        }

        if (stillMomentCount > sleepThreshold) {
            Serial.println("I've been still for a while, going to sleep...");
            enterDeepSleep();
        }
    }
    
    digitalWrite(pinLed, moving ? HIGH : LOW);
    // Broadcast


    bool digitalReadButton = !digitalRead(pinButton);

    if (digitalReadButton == HIGH && !isButtonPressed) {
        isButtonPressed = true;
        Serial.println("Button pressed");
    } else if (digitalReadButton == LOW && isButtonPressed) {
        isButtonPressed = false;
    }

    delay(1);
}