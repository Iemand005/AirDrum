#include <Arduino.h>
#include <RCSwitch.h>
#include <Wire.h>


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
#define pinLed 3
#define pinTransmitter 2

// MPU-9265 I2C address
#define MPU_ADDR 0x68

// MPU-9265 registers
#define REG_PWR_MGMT_1 0x6B
#define REG_ACCEL_CONFIG 0x1C
#define REG_ACCEL_XOUT_H 0x3B
#define REG_WHO_AM_I 0x75

// Threshold for movement detection (tune as needed)
const int threshold = 10;

// Three-axis baseline values for resting position
int16_t baseX, baseY, baseZ;

struct Vec3I {
    int16_t x, y, z;
};

Vec3I lastPos{0,0,0};

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
 * @param x 
 * @param y 
 * @param z 
 */
Vec3I readAccel() {
    Wire.beginTransmission(MPU_ADDR);
    Wire.write(REG_ACCEL_XOUT_H);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU_ADDR, 6);

    // MPU-9265 data is big-endian (high byte first)
    Vec3I pos;
    pos.x = (Wire.read() << 8) | Wire.read();
    pos.y = (Wire.read() << 8) | Wire.read();
    pos.z = (Wire.read() << 8) | Wire.read();
    return pos;
}

// Transmitter code



RCSwitch mySwitch = RCSwitch();

int code = 0;
bool isButtonPressed = false;

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
    readAccel(baseX, baseY, baseZ);
    Serial.print("Baseline - X: "); Serial.print(baseX);
    Serial.print(" Y: "); Serial.print(baseY);
    Serial.println(" Z: "); Serial.println(baseZ);

    Serial.println("Setup complete");

    // Server init

    // Serial.begin(115200);
    Serial.println("Starting Wadda server...");

    pinMode(pinLed, OUTPUT);

    mySwitch.enableTransmit(pinTransmitter);

    Serial.println("Setup complete");
}

void loop() {

    // Reaad accel data
    auto currentPos = readAccel();
    
    // Calculate difference from baseline
    int16_t dx = currentPos.x - lastPos.x;
    int16_t dy = currentPos.y - lastPos.y;
    int16_t dz = currentPos.z - lastPos.z;

    
    // Use sum of absolute differences as movement magnitude
    int magnitude = abs(dx) + abs(dy) + abs(dz);

    magnitude /= 100;

    if (magnitude > 255) magnitude = 255;
    if (magnitude < 0) magnitude = 0;
    
    if (magnitude > threshold) {
        // digitalWrite(pinLed, HIGH);
        analogWrite(pinLed, 10);
        Serial.print("Movement detected! Magnitude: ");
        Serial.println(magnitude);
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

    delay(100);
}