#ifndef SerialCodes_h
#define SerialCodes_h

//#include <Arduino.h>

#include <Arduino.h>

void sendCodeOverSerial(int code, int value) {
    // Print the code to serial as `Message/ { "code": code, "value": value }`
    Serial.print("Message: { \"code\": ");
    Serial.print(code);
    Serial.print(", \"value\": ");
    Serial.print(value);
    Serial.println(" }");
}

#endif